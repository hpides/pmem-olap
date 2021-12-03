// Copyright 2010-2018 Google LLC
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ortools/sat/lp_utils.h"

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "absl/strings/str_cat.h"
#include "ortools/base/int_type.h"
#include "ortools/base/int_type_indexed_vector.h"
#include "ortools/base/integral_types.h"
#include "ortools/base/logging.h"
#include "ortools/glop/lp_solver.h"
#include "ortools/glop/parameters.pb.h"
#include "ortools/lp_data/lp_types.h"
#include "ortools/sat/boolean_problem.h"
#include "ortools/sat/cp_model_utils.h"
#include "ortools/sat/integer.h"
#include "ortools/sat/sat_base.h"
#include "ortools/util/fp_utils.h"

namespace operations_research {
namespace sat {

using glop::ColIndex;
using glop::Fractional;
using glop::kInfinity;
using glop::RowIndex;

using operations_research::MPConstraintProto;
using operations_research::MPModelProto;
using operations_research::MPVariableProto;

namespace {

void ScaleConstraint(const std::vector<double>& var_scaling,
                     MPConstraintProto* mp_constraint) {
  const int num_terms = mp_constraint->coefficient_size();
  for (int i = 0; i < num_terms; ++i) {
    const int var_index = mp_constraint->var_index(i);
    mp_constraint->set_coefficient(
        i, mp_constraint->coefficient(i) / var_scaling[var_index]);
  }
}

void ApplyVarScaling(const std::vector<double> var_scaling,
                     MPModelProto* mp_model) {
  const int num_variables = mp_model->variable_size();
  for (int i = 0; i < num_variables; ++i) {
    const double scaling = var_scaling[i];
    const MPVariableProto& mp_var = mp_model->variable(i);
    const double old_lb = mp_var.lower_bound();
    const double old_ub = mp_var.upper_bound();
    const double old_obj = mp_var.objective_coefficient();
    mp_model->mutable_variable(i)->set_lower_bound(old_lb * scaling);
    mp_model->mutable_variable(i)->set_upper_bound(old_ub * scaling);
    mp_model->mutable_variable(i)->set_objective_coefficient(old_obj / scaling);
  }
  for (MPConstraintProto& mp_constraint : *mp_model->mutable_constraint()) {
    ScaleConstraint(var_scaling, &mp_constraint);
  }
  for (MPGeneralConstraintProto& general_constraint :
       *mp_model->mutable_general_constraint()) {
    switch (general_constraint.general_constraint_case()) {
      case MPGeneralConstraintProto::kIndicatorConstraint:
        ScaleConstraint(var_scaling,
                        general_constraint.mutable_indicator_constraint()
                            ->mutable_constraint());
        break;
      case MPGeneralConstraintProto::kAndConstraint:
      case MPGeneralConstraintProto::kOrConstraint:
        // These constraints have only Boolean variables and no constants. They
        // don't need scaling.
        break;
      default:
        LOG(FATAL) << "Scaling unsupported for general constraint of type "
                   << general_constraint.general_constraint_case();
    }
  }
}

}  // namespace

std::vector<double> ScaleContinuousVariables(double scaling, double max_bound,
                                             MPModelProto* mp_model) {
  const int num_variables = mp_model->variable_size();
  std::vector<double> var_scaling(num_variables, 1.0);
  for (int i = 0; i < num_variables; ++i) {
    if (mp_model->variable(i).is_integer()) continue;
    const double lb = mp_model->variable(i).lower_bound();
    const double ub = mp_model->variable(i).upper_bound();
    const double magnitude = std::max(std::abs(lb), std::abs(ub));
    if (magnitude == 0 || magnitude > max_bound) continue;
    var_scaling[i] = std::min(scaling, max_bound / magnitude);
  }
  ApplyVarScaling(var_scaling, mp_model);
  return var_scaling;
}

// This uses the best rational approximation of x via continuous fractions. It
// is probably not the best implementation, but according to the unit test, it
// seems to do the job.
int FindRationalFactor(double x, int limit, double tolerance) {
  const double initial_x = x;
  x = std::abs(x);
  x -= std::floor(x);
  int q = 1;
  int prev_q = 0;
  while (q < limit) {
    if (std::abs(q * initial_x - std::round(q * initial_x)) < q * tolerance) {
      return q;
    }
    x = 1 / x;
    const int new_q = prev_q + static_cast<int>(std::floor(x)) * q;
    prev_q = q;
    q = new_q;
    x -= std::floor(x);
  }
  return 0;
}

std::vector<double> DetectImpliedIntegers(MPModelProto* mp_model) {
  const int num_variables = mp_model->variable_size();
  std::vector<double> var_scaling(num_variables, 1.0);

  int num_integers = 0;
  for (int i = 0; i < num_variables; ++i) {
    if (mp_model->variable(i).is_integer()) ++num_integers;
  }
  VLOG(1) << "Initial num_integers: " << num_integers << " / " << num_variables;

  // We will process all equality constraints with exactly one non-integer.
  const double tolerance = 1e-6;
  std::vector<int> constraint_queue;

  const int num_constraints = mp_model->constraint_size();
  std::vector<int> constraint_to_num_non_integer(num_constraints, 0);
  std::vector<std::vector<int>> var_to_constraints(num_variables);
  for (int i = 0; i < num_constraints; ++i) {
    const MPConstraintProto& mp_constraint = mp_model->constraint(i);

    // Ignore non-equality for now.
    //
    // TODO(user): If a variable is only bounded in one direction (or if it
    // is the objective variable) we should be able to deal with inequality.
    // See for instance b-ball.mps where only the objective is non-integer. We
    // should really be able to deal with this case. But maybe for the
    // objective, we should just "expand" it before the conversion.
    if (mp_constraint.lower_bound() + tolerance < mp_constraint.upper_bound()) {
      continue;
    }

    for (const int var : mp_constraint.var_index()) {
      if (!mp_model->variable(var).is_integer()) {
        var_to_constraints[var].push_back(i);
        constraint_to_num_non_integer[i]++;
      }
    }
    if (constraint_to_num_non_integer[i] == 1) {
      constraint_queue.push_back(i);
    }
  }
  VLOG(1) << "Initial constraint queue: " << constraint_queue.size() << " / "
          << num_constraints;

  int num_detected = 0;
  int num_fail_due_to_rhs = 0;
  int num_fail_due_to_large_multiplier = 0;
  int num_processed_constraints = 0;
  while (!constraint_queue.empty()) {
    const int top_ct_index = constraint_queue.back();
    constraint_queue.pop_back();

    // The non integer variable was already made integer by one other
    // constraint.
    if (constraint_to_num_non_integer[top_ct_index] == 0) continue;
    ++num_processed_constraints;

    // This will be set to the unique non-integer term of this constraint.
    int var = -1;
    double var_coeff;

    // We are looking for a "multiplier" so that the unique non-integer term
    // in this constraint (i.e. var * var_coeff) times this multiplier is an
    // integer.
    //
    // If this is set to zero or becomes too large, we fail to detect a new
    // implied integer and ignore this constraint.
    double multiplier = 1.0;
    const double max_multiplier = 1e4;

    const MPConstraintProto& ct = mp_model->constraint(top_ct_index);
    const double rhs = ct.lower_bound();
    for (int i = 0; i < ct.var_index().size(); ++i) {
      if (!mp_model->variable(ct.var_index(i)).is_integer()) {
        CHECK_EQ(var, -1);
        var = ct.var_index(i);
        var_coeff = ct.coefficient(i);
      } else {
        // This actually compute the smallest multiplier to make all other
        // terms in the constraint integer.
        const double coeff =
            multiplier * ct.coefficient(i) * var_scaling[ct.var_index(i)];
        multiplier *=
            FindRationalFactor(coeff, /*limit=*/100, multiplier * tolerance);
        if (multiplier == 0 || multiplier > max_multiplier) {
          break;
        }
      }
    }

    if (multiplier == 0 || multiplier > max_multiplier) {
      ++num_fail_due_to_large_multiplier;
      continue;
    }

    // These "rhs" fail could be handled by shifting the variable.
    if (std::abs(std::round(rhs * multiplier) - rhs * multiplier) >
        tolerance * multiplier) {
      ++num_fail_due_to_rhs;
      continue;
    }

    // We want to multiply the variable so that it is integer. We know that
    // coeff * multiplier is an integer, so we just multiply by that.
    const double scaling = std::abs(var_coeff * multiplier);
    ++num_detected;
    CHECK_NE(var, -1);

    VLOG_IF(2, scaling != 1.0) << "Scaled var by " << scaling;

    // Scale the variable right away and mark it as implied integer.
    // Note that the constraints will be scaled later.
    var_scaling[var] = scaling;
    mp_model->mutable_variable(var)->set_is_integer(true);

    // Update the queue of constraints with a single non-integer.
    for (const int ct_index : var_to_constraints[var]) {
      constraint_to_num_non_integer[ct_index]--;
      if (constraint_to_num_non_integer[ct_index] == 1) {
        constraint_queue.push_back(ct_index);
      }
    }
  }

  VLOG(1) << "num_new_integer: " << num_detected
          << " num_processed_constraints: " << num_processed_constraints
          << " num_rhs_fail: " << num_fail_due_to_rhs
          << " num_multiplier_fail: " << num_fail_due_to_large_multiplier;

  ApplyVarScaling(var_scaling, mp_model);
  return var_scaling;
}

namespace {

// We use a class to reuse the temporay memory.
struct ConstraintScaler {
  // Scales an individual constraint.
  ConstraintProto* AddConstraint(const MPModelProto& mp_model,
                                 const MPConstraintProto& mp_constraint,
                                 CpModelProto* cp_model);

  double max_relative_coeff_error = 0.0;
  double max_sum_error = 0.0;
  double max_scaling_factor = 0.0;

  double wanted_precision = 1e-6;
  int64 scaling_target = int64{1} << 50;
  std::vector<int> var_indices;
  std::vector<double> coefficients;
  std::vector<double> lower_bounds;
  std::vector<double> upper_bounds;
};

namespace {

double FindFractionalScaling(const std::vector<double>& coefficients,
                             double tolerance) {
  double multiplier = 1.0;
  for (const double coeff : coefficients) {
    multiplier *=
        FindRationalFactor(coeff, /*limit=*/1e8, multiplier * tolerance);
    if (multiplier == 0.0) break;
  }
  return multiplier;
}

}  // namespace

ConstraintProto* ConstraintScaler::AddConstraint(
    const MPModelProto& mp_model, const MPConstraintProto& mp_constraint,
    CpModelProto* cp_model) {
  if (mp_constraint.lower_bound() == -kInfinity &&
      mp_constraint.upper_bound() == kInfinity) {
    return nullptr;
  }

  auto* constraint = cp_model->add_constraints();
  constraint->set_name(mp_constraint.name());
  auto* arg = constraint->mutable_linear();

  // First scale the coefficients of the constraints so that the constraint
  // sum can always be computed without integer overflow.
  var_indices.clear();
  coefficients.clear();
  lower_bounds.clear();
  upper_bounds.clear();
  const int num_coeffs = mp_constraint.coefficient_size();
  for (int i = 0; i < num_coeffs; ++i) {
    const auto& var_proto = cp_model->variables(mp_constraint.var_index(i));
    const int64 lb = var_proto.domain(0);
    const int64 ub = var_proto.domain(var_proto.domain_size() - 1);
    if (lb == 0 && ub == 0) continue;
    if (mp_constraint.coefficient(i) == 0.0) continue;

    var_indices.push_back(mp_constraint.var_index(i));
    coefficients.push_back(mp_constraint.coefficient(i));
    lower_bounds.push_back(lb);
    upper_bounds.push_back(ub);
  }

  // We use an absolute precision if the constraint domain contains a point in
  // [-1, 1], otherwise we use a relative error to the minimum absolute value
  // in the domain.
  Fractional lb = mp_constraint.lower_bound();
  Fractional ub = mp_constraint.upper_bound();
  double relative_ref = 1.0;
  if (lb > 1.0) relative_ref = lb;
  if (ub < -1.0) relative_ref = -ub;

  double scaling_factor = GetBestScalingOfDoublesToInt64(
      coefficients, lower_bounds, upper_bounds, scaling_target);

  // Returns the smallest factor of the form 2^i that gives us a relative sum
  // error of wanted_precision and still make sure we will have no integer
  // overflow.
  //
  // TODO(user): Make this faster.
  double x = std::min(scaling_factor, 1.0);
  double relative_coeff_error;
  double scaled_sum_error;
  for (; x <= scaling_factor; x *= 2) {
    ComputeScalingErrors(coefficients, lower_bounds, upper_bounds, x,
                         &relative_coeff_error, &scaled_sum_error);
    if (scaled_sum_error < wanted_precision * x * relative_ref) break;
  }
  scaling_factor = x;

  // Because we deal with an approximate input, scaling with a power of 2 might
  // not be the best choice. It is also possible user used rational coeff and
  // then converted them to double (1/2, 1/3, 4/5, etc...). This scaling will
  // recover such rational input and might result in a smaller overall
  // coefficient which is good.
  const double integer_factor = FindFractionalScaling(coefficients, 1e-8);
  if (integer_factor != 0 && integer_factor < scaling_factor) {
    ComputeScalingErrors(coefficients, lower_bounds, upper_bounds, x,
                         &relative_coeff_error, &scaled_sum_error);
    if (scaled_sum_error < wanted_precision * integer_factor * relative_ref) {
      scaling_factor = integer_factor;
    }
  }

  const int64 gcd = ComputeGcdOfRoundedDoubles(coefficients, scaling_factor);
  max_relative_coeff_error =
      std::max(relative_coeff_error, max_relative_coeff_error);
  max_scaling_factor = std::max(scaling_factor / gcd, max_scaling_factor);

  // We do not relax the constraint bound if all variables are integer and
  // we made no error at all during our scaling.
  bool relax_bound = scaled_sum_error > 0;

  for (int i = 0; i < coefficients.size(); ++i) {
    const double scaled_value = coefficients[i] * scaling_factor;
    const int64 value = static_cast<int64>(std::round(scaled_value)) / gcd;
    if (value != 0) {
      if (!mp_model.variable(var_indices[i]).is_integer()) {
        relax_bound = true;
      }
      arg->add_vars(var_indices[i]);
      arg->add_coeffs(value);
    }
  }
  max_sum_error = std::max(max_sum_error,
                           scaled_sum_error / (scaling_factor * relative_ref));

  // Add the constraint bounds. Because we are sure the scaled constraint fit
  // on an int64, if the scaled bounds are too large, the constraint is either
  // always true or always false.
  if (relax_bound) {
    lb -= std::max(1.0, std::abs(lb)) * wanted_precision;
  }
  const Fractional scaled_lb = std::ceil(lb * scaling_factor);
  if (lb == -kInfinity || scaled_lb <= kint64min) {
    arg->add_domain(kint64min);
  } else {
    arg->add_domain(CeilRatio(IntegerValue(static_cast<int64>(scaled_lb)),
                              IntegerValue(gcd))
                        .value());
  }

  if (relax_bound) {
    ub += std::max(1.0, std::abs(ub)) * wanted_precision;
  }
  const Fractional scaled_ub = std::floor(ub * scaling_factor);
  if (ub == kInfinity || scaled_ub >= kint64max) {
    arg->add_domain(kint64max);
  } else {
    arg->add_domain(FloorRatio(IntegerValue(static_cast<int64>(scaled_ub)),
                               IntegerValue(gcd))
                        .value());
  }

  return constraint;
}

}  // namespace

bool ConvertMPModelProtoToCpModelProto(const SatParameters& params,
                                       const MPModelProto& mp_model,
                                       CpModelProto* cp_model) {
  CHECK(cp_model != nullptr);
  cp_model->Clear();
  cp_model->set_name(mp_model.name());

  // To make sure we cannot have integer overflow, we use this bound for any
  // unbounded variable.
  //
  // TODO(user): This could be made larger if needed, so be smarter if we have
  // MIP problem that we cannot "convert" because of this. Note however than we
  // cannot go that much further because we need to make sure we will not run
  // into overflow if we add a big linear combination of such variables. It
  // should always be possible for a user to scale its problem so that all
  // relevant quantities are a couple of millions. A LP/MIP solver have a
  // similar condition in disguise because problem with a difference of more
  // than 6 magnitudes between the variable values will likely run into numeric
  // trouble.
  const int64 kMaxVariableBound = static_cast<int64>(params.mip_max_bound());

  int num_truncated_bounds = 0;
  int num_small_domains = 0;
  const int64 kSmallDomainSize = 1000;
  const double kWantedPrecision = params.mip_wanted_precision();

  // Add the variables.
  const int num_variables = mp_model.variable_size();
  for (int i = 0; i < num_variables; ++i) {
    const MPVariableProto& mp_var = mp_model.variable(i);
    IntegerVariableProto* cp_var = cp_model->add_variables();
    cp_var->set_name(mp_var.name());

    // Deal with the corner case of a domain far away from zero.
    //
    // TODO(user): We should deal with these case by shifting the domain so
    // that it includes zero instead of just fixing the variable. But that is a
    // bit of work as it requires some postsolve.
    if (mp_var.lower_bound() > kMaxVariableBound) {
      // Fix var to its lower bound.
      ++num_truncated_bounds;
      const int64 value = static_cast<int64>(std::round(mp_var.lower_bound()));
      cp_var->add_domain(value);
      cp_var->add_domain(value);
      continue;
    } else if (mp_var.upper_bound() < -kMaxVariableBound) {
      // Fix var to its upper_bound.
      ++num_truncated_bounds;
      const int64 value = static_cast<int64>(std::round(mp_var.upper_bound()));
      cp_var->add_domain(value);
      cp_var->add_domain(value);
      continue;
    }

    // Note that we must process the lower bound first.
    for (const bool lower : {true, false}) {
      const double bound = lower ? mp_var.lower_bound() : mp_var.upper_bound();
      if (std::abs(bound) >= kMaxVariableBound) {
        ++num_truncated_bounds;
        cp_var->add_domain(bound < 0 ? -kMaxVariableBound : kMaxVariableBound);
        continue;
      }

      // Note that the cast is "perfect" because we forbid large values.
      cp_var->add_domain(
          static_cast<int64>(lower ? std::ceil(bound - kWantedPrecision)
                                   : std::floor(bound + kWantedPrecision)));
    }

    // Notify if a continuous variable has a small domain as this is likely to
    // make an all integer solution far from a continuous one.
    if (!mp_var.is_integer() && cp_var->domain(0) != cp_var->domain(1) &&
        cp_var->domain(1) - cp_var->domain(0) < kSmallDomainSize) {
      ++num_small_domains;
    }
  }

  LOG_IF(WARNING, num_truncated_bounds > 0)
      << num_truncated_bounds << " bounds were truncated to "
      << kMaxVariableBound << ".";
  LOG_IF(WARNING, num_small_domains > 0)
      << num_small_domains << " continuous variable domain with fewer than "
      << kSmallDomainSize << " values.";

  ConstraintScaler scaler;
  const int64 kScalingTarget = int64{1} << params.mip_max_activity_exponent();
  scaler.wanted_precision = kWantedPrecision;
  scaler.scaling_target = kScalingTarget;

  // Add the constraints. We scale each of them individually.
  for (const MPConstraintProto& mp_constraint : mp_model.constraint()) {
    scaler.AddConstraint(mp_model, mp_constraint, cp_model);
  }
  for (const MPGeneralConstraintProto& general_constraint :
       mp_model.general_constraint()) {
    switch (general_constraint.general_constraint_case()) {
      case MPGeneralConstraintProto::kIndicatorConstraint: {
        const auto& indicator_constraint =
            general_constraint.indicator_constraint();
        const MPConstraintProto& mp_constraint =
            indicator_constraint.constraint();
        ConstraintProto* ct =
            scaler.AddConstraint(mp_model, mp_constraint, cp_model);
        if (ct == nullptr) continue;

        // Add the indicator.
        const int var = indicator_constraint.var_index();
        const int value = indicator_constraint.var_value();
        ct->add_enforcement_literal(value == 1 ? var : NegatedRef(var));
        break;
      }
      case MPGeneralConstraintProto::kAndConstraint: {
        const auto& and_constraint = general_constraint.and_constraint();
        const std::string& name = general_constraint.name();

        ConstraintProto* ct_pos = cp_model->add_constraints();
        ct_pos->set_name(name.empty() ? "" : absl::StrCat(name, "_pos"));
        ct_pos->add_enforcement_literal(and_constraint.resultant_var_index());
        *ct_pos->mutable_bool_and()->mutable_literals() =
            and_constraint.var_index();

        ConstraintProto* ct_neg = cp_model->add_constraints();
        ct_neg->set_name(name.empty() ? "" : absl::StrCat(name, "_neg"));
        ct_neg->add_enforcement_literal(
            NegatedRef(and_constraint.resultant_var_index()));
        for (const int var_index : and_constraint.var_index()) {
          ct_neg->mutable_bool_or()->add_literals(NegatedRef(var_index));
        }
        break;
      }
      case MPGeneralConstraintProto::kOrConstraint: {
        const auto& or_constraint = general_constraint.or_constraint();
        const std::string& name = general_constraint.name();

        ConstraintProto* ct_pos = cp_model->add_constraints();
        ct_pos->set_name(name.empty() ? "" : absl::StrCat(name, "_pos"));
        ct_pos->add_enforcement_literal(or_constraint.resultant_var_index());
        *ct_pos->mutable_bool_or()->mutable_literals() =
            or_constraint.var_index();

        ConstraintProto* ct_neg = cp_model->add_constraints();
        ct_neg->set_name(name.empty() ? "" : absl::StrCat(name, "_neg"));
        ct_neg->add_enforcement_literal(
            NegatedRef(or_constraint.resultant_var_index()));
        for (const int var_index : or_constraint.var_index()) {
          ct_neg->mutable_bool_and()->add_literals(NegatedRef(var_index));
        }
        break;
      }
      default:
        LOG(ERROR) << "Can't convert general constraints of type "
                   << general_constraint.general_constraint_case()
                   << " to CpModelProto.";
        return false;
    }
  }

  double max_relative_coeff_error = scaler.max_relative_coeff_error;
  double max_sum_error = scaler.max_sum_error;
  double max_scaling_factor = scaler.max_scaling_factor;

  // Display the error/scaling without taking into account the objective first.
  VLOG(1) << "Maximum constraint coefficient relative error: "
          << max_relative_coeff_error;
  VLOG(1) << "Maximum constraint worst-case sum absolute error: "
          << max_sum_error;
  VLOG(1) << "Maximum constraint scaling factor: " << max_scaling_factor;

  // Add the objective.
  std::vector<int> var_indices;
  std::vector<double> coefficients;
  std::vector<double> lower_bounds;
  std::vector<double> upper_bounds;
  for (int i = 0; i < num_variables; ++i) {
    const MPVariableProto& mp_var = mp_model.variable(i);
    if (mp_var.objective_coefficient() == 0.0) continue;

    const auto& var_proto = cp_model->variables(i);
    const int64 lb = var_proto.domain(0);
    const int64 ub = var_proto.domain(var_proto.domain_size() - 1);
    if (lb == 0 && ub == 0) continue;

    var_indices.push_back(i);
    coefficients.push_back(mp_var.objective_coefficient());
    lower_bounds.push_back(lb);
    upper_bounds.push_back(ub);
  }
  if (!coefficients.empty() || mp_model.objective_offset() != 0.0) {
    double scaling_factor = GetBestScalingOfDoublesToInt64(
        coefficients, lower_bounds, upper_bounds, kScalingTarget);

    // Returns the smallest factor of the form 2^i that gives us an absolute
    // error of kWantedPrecision and still make sure we will have no integer
    // overflow.
    //
    // TODO(user): Make this faster.
    double x = std::min(scaling_factor, 1.0);
    double relative_coeff_error;
    double scaled_sum_error;
    for (; x <= scaling_factor; x *= 2) {
      ComputeScalingErrors(coefficients, lower_bounds, upper_bounds, x,
                           &relative_coeff_error, &scaled_sum_error);
      if (scaled_sum_error < kWantedPrecision * x) break;
    }
    scaling_factor = x;

    // Same remark as for the constraint.
    // TODO(user): Extract common code.
    const double integer_factor = FindFractionalScaling(coefficients, 1e-8);
    if (integer_factor != 0 && integer_factor < scaling_factor) {
      ComputeScalingErrors(coefficients, lower_bounds, upper_bounds, x,
                           &relative_coeff_error, &scaled_sum_error);
      if (scaled_sum_error < kWantedPrecision * integer_factor) {
        scaling_factor = integer_factor;
      }
    }

    const int64 gcd = ComputeGcdOfRoundedDoubles(coefficients, scaling_factor);
    max_relative_coeff_error =
        std::max(relative_coeff_error, max_relative_coeff_error);

    // Display the objective error/scaling.
    VLOG(1) << "objective coefficient relative error: " << relative_coeff_error;
    VLOG(1) << "objective worst-case absolute error: "
            << scaled_sum_error / scaling_factor;
    VLOG(1) << "objective scaling factor: " << scaling_factor / gcd;

    // Note that here we set the scaling factor for the inverse operation of
    // getting the "true" objective value from the scaled one. Hence the
    // inverse.
    auto* objective = cp_model->mutable_objective();
    const int mult = mp_model.maximize() ? -1 : 1;
    objective->set_offset(mp_model.objective_offset() * scaling_factor / gcd *
                          mult);
    objective->set_scaling_factor(1.0 / scaling_factor * gcd * mult);
    for (int i = 0; i < coefficients.size(); ++i) {
      const int64 value =
          static_cast<int64>(std::round(coefficients[i] * scaling_factor)) /
          gcd;
      if (value != 0) {
        objective->add_vars(var_indices[i]);
        objective->add_coeffs(value * mult);
      }
    }
  }

  // Test the precision of the conversion.
  const double allowed_error =
      std::max(params.mip_check_precision(), params.mip_wanted_precision());
  if (max_sum_error > allowed_error) {
    LOG(WARNING) << "The relative error during double -> int64 conversion "
                 << "is too high! error:" << max_sum_error
                 << " check_tolerance:" << allowed_error;
    return false;
  }
  return true;
}

bool ConvertBinaryMPModelProtoToBooleanProblem(const MPModelProto& mp_model,
                                               LinearBooleanProblem* problem) {
  CHECK(problem != nullptr);
  problem->Clear();
  problem->set_name(mp_model.name());
  const int num_variables = mp_model.variable_size();
  problem->set_num_variables(num_variables);

  // Test if the variables are binary variables.
  // Add constraints for the fixed variables.
  for (int var_id(0); var_id < num_variables; ++var_id) {
    const MPVariableProto& mp_var = mp_model.variable(var_id);
    problem->add_var_names(mp_var.name());

    // This will be changed to false as soon as we detect the variable to be
    // non-binary. This is done this way so we can display a nice error message
    // before aborting the function and returning false.
    bool is_binary = mp_var.is_integer();

    const Fractional lb = mp_var.lower_bound();
    const Fractional ub = mp_var.upper_bound();
    if (lb <= -1.0) is_binary = false;
    if (ub >= 2.0) is_binary = false;
    if (is_binary) {
      // 4 cases.
      if (lb <= 0.0 && ub >= 1.0) {
        // Binary variable. Ok.
      } else if (lb <= 1.0 && ub >= 1.0) {
        // Fixed variable at 1.
        LinearBooleanConstraint* constraint = problem->add_constraints();
        constraint->set_lower_bound(1);
        constraint->set_upper_bound(1);
        constraint->add_literals(var_id + 1);
        constraint->add_coefficients(1);
      } else if (lb <= 0.0 && ub >= 0.0) {
        // Fixed variable at 0.
        LinearBooleanConstraint* constraint = problem->add_constraints();
        constraint->set_lower_bound(0);
        constraint->set_upper_bound(0);
        constraint->add_literals(var_id + 1);
        constraint->add_coefficients(1);
      } else {
        // No possible integer value!
        is_binary = false;
      }
    }

    // Abort if the variable is not binary.
    if (!is_binary) {
      LOG(WARNING) << "The variable #" << var_id << " with name "
                   << mp_var.name() << " is not binary. "
                   << "lb: " << lb << " ub: " << ub;
      return false;
    }
  }

  // Variables needed to scale the double coefficients into int64.
  const int64 kInt64Max = std::numeric_limits<int64>::max();
  double max_relative_error = 0.0;
  double max_bound_error = 0.0;
  double max_scaling_factor = 0.0;
  double relative_error = 0.0;
  double scaling_factor = 0.0;
  std::vector<double> coefficients;

  // Add all constraints.
  for (const MPConstraintProto& mp_constraint : mp_model.constraint()) {
    LinearBooleanConstraint* constraint = problem->add_constraints();
    constraint->set_name(mp_constraint.name());

    // First scale the coefficients of the constraints.
    coefficients.clear();
    const int num_coeffs = mp_constraint.coefficient_size();
    for (int i = 0; i < num_coeffs; ++i) {
      coefficients.push_back(mp_constraint.coefficient(i));
    }
    GetBestScalingOfDoublesToInt64(coefficients, kInt64Max, &scaling_factor,
                                   &relative_error);
    const int64 gcd = ComputeGcdOfRoundedDoubles(coefficients, scaling_factor);
    max_relative_error = std::max(relative_error, max_relative_error);
    max_scaling_factor = std::max(scaling_factor / gcd, max_scaling_factor);

    double bound_error = 0.0;
    for (int i = 0; i < num_coeffs; ++i) {
      const double scaled_value = mp_constraint.coefficient(i) * scaling_factor;
      bound_error += std::abs(round(scaled_value) - scaled_value);
      const int64 value = static_cast<int64>(round(scaled_value)) / gcd;
      if (value != 0) {
        constraint->add_literals(mp_constraint.var_index(i) + 1);
        constraint->add_coefficients(value);
      }
    }
    max_bound_error = std::max(max_bound_error, bound_error);

    // Add the bounds. Note that we do not pass them to
    // GetBestScalingOfDoublesToInt64() because we know that the sum of absolute
    // coefficients of the constraint fit on an int64. If one of the scaled
    // bound overflows, we don't care by how much because in this case the
    // constraint is just trivial or unsatisfiable.
    const Fractional lb = mp_constraint.lower_bound();
    if (lb != -kInfinity) {
      if (lb * scaling_factor > static_cast<double>(kInt64Max)) {
        LOG(WARNING) << "A constraint is trivially unsatisfiable.";
        return false;
      }
      if (lb * scaling_factor > -static_cast<double>(kInt64Max)) {
        // Otherwise, the constraint is not needed.
        constraint->set_lower_bound(
            static_cast<int64>(round(lb * scaling_factor - bound_error)) / gcd);
      }
    }
    const Fractional ub = mp_constraint.upper_bound();
    if (ub != kInfinity) {
      if (ub * scaling_factor < -static_cast<double>(kInt64Max)) {
        LOG(WARNING) << "A constraint is trivially unsatisfiable.";
        return false;
      }
      if (ub * scaling_factor < static_cast<double>(kInt64Max)) {
        // Otherwise, the constraint is not needed.
        constraint->set_upper_bound(
            static_cast<int64>(round(ub * scaling_factor + bound_error)) / gcd);
      }
    }
  }

  // Display the error/scaling without taking into account the objective first.
  LOG(INFO) << "Maximum constraint relative error: " << max_relative_error;
  LOG(INFO) << "Maximum constraint bound error: " << max_bound_error;
  LOG(INFO) << "Maximum constraint scaling factor: " << max_scaling_factor;

  // Add the objective.
  coefficients.clear();
  for (int var_id = 0; var_id < num_variables; ++var_id) {
    const MPVariableProto& mp_var = mp_model.variable(var_id);
    coefficients.push_back(mp_var.objective_coefficient());
  }
  GetBestScalingOfDoublesToInt64(coefficients, kInt64Max, &scaling_factor,
                                 &relative_error);
  const int64 gcd = ComputeGcdOfRoundedDoubles(coefficients, scaling_factor);
  max_relative_error = std::max(relative_error, max_relative_error);

  // Display the objective error/scaling.
  LOG(INFO) << "objective relative error: " << relative_error;
  LOG(INFO) << "objective scaling factor: " << scaling_factor / gcd;

  LinearObjective* objective = problem->mutable_objective();
  objective->set_offset(mp_model.objective_offset() * scaling_factor / gcd);

  // Note that here we set the scaling factor for the inverse operation of
  // getting the "true" objective value from the scaled one. Hence the inverse.
  objective->set_scaling_factor(1.0 / scaling_factor * gcd);
  for (int var_id = 0; var_id < num_variables; ++var_id) {
    const MPVariableProto& mp_var = mp_model.variable(var_id);
    const int64 value = static_cast<int64>(round(
                            mp_var.objective_coefficient() * scaling_factor)) /
                        gcd;
    if (value != 0) {
      objective->add_literals(var_id + 1);
      objective->add_coefficients(value);
    }
  }

  // If the problem was a maximization one, we need to modify the objective.
  if (mp_model.maximize()) ChangeOptimizationDirection(problem);

  // Test the precision of the conversion.
  const double kRelativeTolerance = 1e-8;
  if (max_relative_error > kRelativeTolerance) {
    LOG(WARNING) << "The relative error during double -> int64 conversion "
                 << "is too high!";
    return false;
  }
  return true;
}

void ConvertBooleanProblemToLinearProgram(const LinearBooleanProblem& problem,
                                          glop::LinearProgram* lp) {
  lp->Clear();
  for (int i = 0; i < problem.num_variables(); ++i) {
    const ColIndex col = lp->CreateNewVariable();
    lp->SetVariableType(col, glop::LinearProgram::VariableType::INTEGER);
    lp->SetVariableBounds(col, 0.0, 1.0);
  }

  // Variables name are optional.
  if (problem.var_names_size() != 0) {
    CHECK_EQ(problem.var_names_size(), problem.num_variables());
    for (int i = 0; i < problem.num_variables(); ++i) {
      lp->SetVariableName(ColIndex(i), problem.var_names(i));
    }
  }

  for (const LinearBooleanConstraint& constraint : problem.constraints()) {
    const RowIndex constraint_index = lp->CreateNewConstraint();
    lp->SetConstraintName(constraint_index, constraint.name());
    double sum = 0.0;
    for (int i = 0; i < constraint.literals_size(); ++i) {
      const int literal = constraint.literals(i);
      const double coeff = constraint.coefficients(i);
      const ColIndex variable_index = ColIndex(abs(literal) - 1);
      if (literal < 0) {
        sum += coeff;
        lp->SetCoefficient(constraint_index, variable_index, -coeff);
      } else {
        lp->SetCoefficient(constraint_index, variable_index, coeff);
      }
    }
    lp->SetConstraintBounds(
        constraint_index,
        constraint.has_lower_bound() ? constraint.lower_bound() - sum
                                     : -kInfinity,
        constraint.has_upper_bound() ? constraint.upper_bound() - sum
                                     : kInfinity);
  }

  // Objective.
  {
    double sum = 0.0;
    const LinearObjective& objective = problem.objective();
    const double scaling_factor = objective.scaling_factor();
    for (int i = 0; i < objective.literals_size(); ++i) {
      const int literal = objective.literals(i);
      const double coeff =
          static_cast<double>(objective.coefficients(i)) * scaling_factor;
      const ColIndex variable_index = ColIndex(abs(literal) - 1);
      if (literal < 0) {
        sum += coeff;
        lp->SetObjectiveCoefficient(variable_index, -coeff);
      } else {
        lp->SetObjectiveCoefficient(variable_index, coeff);
      }
    }
    lp->SetObjectiveOffset((sum + objective.offset()) * scaling_factor);
    lp->SetMaximizationProblem(scaling_factor < 0);
  }

  lp->CleanUp();
}

int FixVariablesFromSat(const SatSolver& solver, glop::LinearProgram* lp) {
  int num_fixed_variables = 0;
  const Trail& trail = solver.LiteralTrail();
  for (int i = 0; i < trail.Index(); ++i) {
    const BooleanVariable var = trail[i].Variable();
    const int value = trail[i].IsPositive() ? 1.0 : 0.0;
    if (trail.Info(var).level == 0) {
      ++num_fixed_variables;
      lp->SetVariableBounds(ColIndex(var.value()), value, value);
    }
  }
  return num_fixed_variables;
}

bool SolveLpAndUseSolutionForSatAssignmentPreference(
    const glop::LinearProgram& lp, SatSolver* sat_solver,
    double max_time_in_seconds) {
  glop::LPSolver solver;
  glop::GlopParameters glop_parameters;
  glop_parameters.set_max_time_in_seconds(max_time_in_seconds);
  solver.SetParameters(glop_parameters);
  const glop::ProblemStatus& status = solver.Solve(lp);
  if (status != glop::ProblemStatus::OPTIMAL &&
      status != glop::ProblemStatus::IMPRECISE &&
      status != glop::ProblemStatus::PRIMAL_FEASIBLE) {
    return false;
  }
  for (ColIndex col(0); col < lp.num_variables(); ++col) {
    const Fractional& value = solver.variable_values()[col];
    sat_solver->SetAssignmentPreference(
        Literal(BooleanVariable(col.value()), round(value) == 1),
        1 - std::abs(value - round(value)));
  }
  return true;
}

bool SolveLpAndUseIntegerVariableToStartLNS(const glop::LinearProgram& lp,
                                            LinearBooleanProblem* problem) {
  glop::LPSolver solver;
  const glop::ProblemStatus& status = solver.Solve(lp);
  if (status != glop::ProblemStatus::OPTIMAL &&
      status != glop::ProblemStatus::PRIMAL_FEASIBLE)
    return false;
  int num_variable_fixed = 0;
  for (ColIndex col(0); col < lp.num_variables(); ++col) {
    const Fractional tolerance = 1e-5;
    const Fractional& value = solver.variable_values()[col];
    if (value > 1 - tolerance) {
      ++num_variable_fixed;
      LinearBooleanConstraint* constraint = problem->add_constraints();
      constraint->set_lower_bound(1);
      constraint->set_upper_bound(1);
      constraint->add_coefficients(1);
      constraint->add_literals(col.value() + 1);
    } else if (value < tolerance) {
      ++num_variable_fixed;
      LinearBooleanConstraint* constraint = problem->add_constraints();
      constraint->set_lower_bound(0);
      constraint->set_upper_bound(0);
      constraint->add_coefficients(1);
      constraint->add_literals(col.value() + 1);
    }
  }
  LOG(INFO) << "LNS with " << num_variable_fixed << " fixed variables.";
  return true;
}

}  // namespace sat
}  // namespace operations_research
