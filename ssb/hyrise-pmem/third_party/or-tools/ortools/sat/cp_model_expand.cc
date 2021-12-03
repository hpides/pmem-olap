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

#include "ortools/sat/cp_model_expand.h"

#include <map>

#include "absl/container/flat_hash_map.h"
#include "ortools/base/hash.h"
#include "ortools/base/map_util.h"
#include "ortools/base/stl_util.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_utils.h"
#include "ortools/sat/presolve_context.h"
#include "ortools/sat/util.h"
#include "ortools/util/saturated_arithmetic.h"
#include "ortools/util/sorted_interval_list.h"

namespace operations_research {
namespace sat {
namespace {

void ExpandReservoir(ConstraintProto* ct, PresolveContext* context) {
  if (ct->reservoir().min_level() > ct->reservoir().max_level()) {
    VLOG(1) << "Empty level domain in reservoir constraint.";
    return (void)context->NotifyThatModelIsUnsat();
  }

  // TODO(user): Support sharing constraints in the model across constraints.
  absl::flat_hash_map<std::pair<int, int>, int> precedence_cache;
  const ReservoirConstraintProto& reservoir = ct->reservoir();
  const int num_events = reservoir.times_size();

  const int true_literal = context->GetOrCreateConstantVar(1);

  const auto is_active_literal = [&reservoir, true_literal](int index) {
    if (reservoir.actives_size() == 0) return true_literal;
    return reservoir.actives(index);
  };

  // x_lesseq_y <=> (x <= y && l_x is true && l_y is true).
  const auto add_reified_precedence = [&context](int x_lesseq_y, int x, int y,
                                                 int l_x, int l_y) {
    // x_lesseq_y => (x <= y) && l_x is true && l_y is true.
    ConstraintProto* const lesseq = context->working_model->add_constraints();
    lesseq->add_enforcement_literal(x_lesseq_y);
    lesseq->mutable_linear()->add_vars(x);
    lesseq->mutable_linear()->add_vars(y);
    lesseq->mutable_linear()->add_coeffs(-1);
    lesseq->mutable_linear()->add_coeffs(1);
    lesseq->mutable_linear()->add_domain(0);
    lesseq->mutable_linear()->add_domain(kint64max);
    if (!context->LiteralIsTrue(l_x)) {
      context->AddImplication(x_lesseq_y, l_x);
    }
    if (!context->LiteralIsTrue(l_y)) {
      context->AddImplication(x_lesseq_y, l_y);
    }

    // Not(x_lesseq_y) && l_x && l_y => (x > y)
    ConstraintProto* const greater = context->working_model->add_constraints();
    greater->mutable_linear()->add_vars(x);
    greater->mutable_linear()->add_vars(y);
    greater->mutable_linear()->add_coeffs(-1);
    greater->mutable_linear()->add_coeffs(1);
    greater->mutable_linear()->add_domain(kint64min);
    greater->mutable_linear()->add_domain(-1);

    // Manages enforcement literal.
    greater->add_enforcement_literal(NegatedRef(x_lesseq_y));
    greater->add_enforcement_literal(l_x);
    greater->add_enforcement_literal(l_y);
  };

  int num_positives = 0;
  int num_negatives = 0;
  for (const int64 demand : reservoir.demands()) {
    if (demand > 0) {
      num_positives++;
    } else if (demand < 0) {
      num_negatives++;
    }
  }

  if (num_positives > 0 && num_negatives > 0) {
    // Creates Boolean variables equivalent to (start[i] <= start[j]) i != j
    for (int i = 0; i < num_events - 1; ++i) {
      const int active_i = is_active_literal(i);
      if (context->LiteralIsFalse(active_i)) continue;

      const int time_i = reservoir.times(i);
      for (int j = i + 1; j < num_events; ++j) {
        const int active_j = is_active_literal(j);
        if (context->LiteralIsFalse(active_j)) continue;

        const int time_j = reservoir.times(j);
        const std::pair<int, int> p = std::make_pair(time_i, time_j);
        const std::pair<int, int> rev_p = std::make_pair(time_j, time_i);
        if (gtl::ContainsKey(precedence_cache, p)) continue;

        const int i_lesseq_j = context->NewBoolVar();
        context->working_model->mutable_variables(i_lesseq_j)
            ->set_name(absl::StrCat(i, " before ", j));
        precedence_cache[p] = i_lesseq_j;
        const int j_lesseq_i = context->NewBoolVar();
        context->working_model->mutable_variables(j_lesseq_i)
            ->set_name(absl::StrCat(j, " before ", i));

        precedence_cache[rev_p] = j_lesseq_i;
        add_reified_precedence(i_lesseq_j, time_i, time_j, active_i, active_j);
        add_reified_precedence(j_lesseq_i, time_j, time_i, active_j, active_i);

        // Consistency. This is redundant but should improves performance.
        auto* const bool_or =
            context->working_model->add_constraints()->mutable_bool_or();
        bool_or->add_literals(i_lesseq_j);
        bool_or->add_literals(j_lesseq_i);
        bool_or->add_literals(NegatedRef(active_i));
        bool_or->add_literals(NegatedRef(active_j));
      }
    }

    // Constrains the running level to be consistent at all times.
    // For this we only add a constraint at the time a given demand
    // take place. We also have a constraint for time zero if needed
    // (added below).
    for (int i = 0; i < num_events; ++i) {
      const int active_i = is_active_literal(i);
      if (context->LiteralIsFalse(active_i)) continue;
      const int time_i = reservoir.times(i);

      // Accumulates demands of all predecessors.
      ConstraintProto* const level = context->working_model->add_constraints();
      level->add_enforcement_literal(active_i);

      // Add contributions from previous events.
      for (int j = 0; j < num_events; ++j) {
        if (i == j) continue;
        const int active_j = is_active_literal(j);
        if (context->LiteralIsFalse(active_j)) continue;

        const int time_j = reservoir.times(j);
        level->mutable_linear()->add_vars(gtl::FindOrDieNoPrint(
            precedence_cache, std::make_pair(time_j, time_i)));
        level->mutable_linear()->add_coeffs(reservoir.demands(j));
      }

      // Accounts for own demand in the domain of the sum.
      const int64 demand_i = reservoir.demands(i);
      level->mutable_linear()->add_domain(
          CapSub(reservoir.min_level(), demand_i));
      level->mutable_linear()->add_domain(
          CapSub(reservoir.max_level(), demand_i));
    }
  } else {
    // If all demands have the same sign, we do not care about the order, just
    // the sum.
    auto* const sum =
        context->working_model->add_constraints()->mutable_linear();
    for (int i = 0; i < num_events; ++i) {
      sum->add_vars(is_active_literal(i));
      sum->add_coeffs(reservoir.demands(i));
    }
    sum->add_domain(reservoir.min_level());
    sum->add_domain(reservoir.max_level());
  }

  // Constrains the reservoir level to be consistent at time 0.
  // We need to do it only if 0 is not in [min_level..max_level].
  // Otherwise, the regular propagation will already check it.
  if (reservoir.min_level() > 0 || reservoir.max_level() < 0) {
    auto* const sum_at_zero =
        context->working_model->add_constraints()->mutable_linear();
    for (int i = 0; i < num_events; ++i) {
      const int active_i = is_active_literal(i);
      if (context->LiteralIsFalse(active_i)) continue;

      const int time_i = reservoir.times(i);
      const int lesseq_0 = context->NewBoolVar();

      // lesseq_0 => (time_i <= 0) && active_i is true
      ConstraintProto* const lesseq = context->working_model->add_constraints();
      lesseq->add_enforcement_literal(lesseq_0);
      lesseq->mutable_linear()->add_vars(time_i);
      lesseq->mutable_linear()->add_coeffs(1);
      lesseq->mutable_linear()->add_domain(kint64min);
      lesseq->mutable_linear()->add_domain(0);

      if (!context->LiteralIsTrue(active_i)) {
        context->AddImplication(lesseq_0, active_i);
      }

      // Not(lesseq_0) && active_i => (time_i >= 1)
      ConstraintProto* const greater =
          context->working_model->add_constraints();
      greater->add_enforcement_literal(NegatedRef(lesseq_0));
      greater->add_enforcement_literal(active_i);
      greater->mutable_linear()->add_vars(time_i);
      greater->mutable_linear()->add_coeffs(1);
      greater->mutable_linear()->add_domain(1);
      greater->mutable_linear()->add_domain(kint64max);

      // Accumulate in the sum_at_zero constraint.
      sum_at_zero->add_vars(lesseq_0);
      sum_at_zero->add_coeffs(reservoir.demands(i));
    }
    sum_at_zero->add_domain(reservoir.min_level());
    sum_at_zero->add_domain(reservoir.max_level());
  }

  ct->Clear();
  context->UpdateRuleStats("reservoir: expanded");
}

void ExpandIntMod(ConstraintProto* ct, PresolveContext* context) {
  const IntegerArgumentProto& int_mod = ct->int_mod();
  const int var = int_mod.vars(0);
  const int mod_var = int_mod.vars(1);
  const int target_var = int_mod.target();

  const int64 mod_lb = context->MinOf(mod_var);
  CHECK_GE(mod_lb, 1);
  const int64 mod_ub = context->MaxOf(mod_var);

  const int64 var_lb = context->MinOf(var);
  const int64 var_ub = context->MaxOf(var);

  // Compute domains of var / mod_var.
  const int div_var =
      context->NewIntVar(Domain(var_lb / mod_ub, var_ub / mod_lb));

  auto add_enforcement_literal_if_needed = [&]() {
    if (ct->enforcement_literal_size() == 0) return;
    const int literal = ct->enforcement_literal(0);
    ConstraintProto* const last = context->working_model->mutable_constraints(
        context->working_model->constraints_size() - 1);
    last->add_enforcement_literal(literal);
  };

  // div = var / mod.
  IntegerArgumentProto* const div_proto =
      context->working_model->add_constraints()->mutable_int_div();
  div_proto->set_target(div_var);
  div_proto->add_vars(var);
  div_proto->add_vars(mod_var);
  add_enforcement_literal_if_needed();

  // Checks if mod is constant.
  if (mod_lb == mod_ub) {
    // var - div_var * mod = target.
    LinearConstraintProto* const lin =
        context->working_model->add_constraints()->mutable_linear();
    lin->add_vars(int_mod.vars(0));
    lin->add_coeffs(1);
    lin->add_vars(div_var);
    lin->add_coeffs(-mod_lb);
    lin->add_vars(target_var);
    lin->add_coeffs(-1);
    lin->add_domain(0);
    lin->add_domain(0);
    add_enforcement_literal_if_needed();
  } else {
    // Create prod_var = div_var * mod.
    const int prod_var = context->NewIntVar(
        Domain(var_lb * mod_lb / mod_ub, var_ub * mod_ub / mod_lb));
    IntegerArgumentProto* const int_prod =
        context->working_model->add_constraints()->mutable_int_prod();
    int_prod->set_target(prod_var);
    int_prod->add_vars(div_var);
    int_prod->add_vars(mod_var);
    add_enforcement_literal_if_needed();

    // var - prod_var = target.
    LinearConstraintProto* const lin =
        context->working_model->add_constraints()->mutable_linear();
    lin->add_vars(var);
    lin->add_coeffs(1);
    lin->add_vars(prod_var);
    lin->add_coeffs(-1);
    lin->add_vars(target_var);
    lin->add_coeffs(-1);
    lin->add_domain(0);
    lin->add_domain(0);
    add_enforcement_literal_if_needed();
  }

  ct->Clear();
  context->UpdateRuleStats("int_mod: expanded");
}

void ExpandIntProdWithBoolean(int bool_ref, int int_ref, int product_ref,
                              PresolveContext* context) {
  ConstraintProto* const one = context->working_model->add_constraints();
  one->add_enforcement_literal(bool_ref);
  one->mutable_linear()->add_vars(int_ref);
  one->mutable_linear()->add_coeffs(1);
  one->mutable_linear()->add_vars(product_ref);
  one->mutable_linear()->add_coeffs(-1);
  one->mutable_linear()->add_domain(0);
  one->mutable_linear()->add_domain(0);

  ConstraintProto* const zero = context->working_model->add_constraints();
  zero->add_enforcement_literal(NegatedRef(bool_ref));
  zero->mutable_linear()->add_vars(product_ref);
  zero->mutable_linear()->add_coeffs(1);
  zero->mutable_linear()->add_domain(0);
  zero->mutable_linear()->add_domain(0);
}

void ExpandIntProd(ConstraintProto* ct, PresolveContext* context) {
  const IntegerArgumentProto& int_prod = ct->int_prod();
  if (int_prod.vars_size() != 2) return;
  const int a = int_prod.vars(0);
  const int b = int_prod.vars(1);
  const int p = int_prod.target();
  const bool a_is_boolean =
      RefIsPositive(a) && context->MinOf(a) == 0 && context->MaxOf(a) == 1;
  const bool b_is_boolean =
      RefIsPositive(b) && context->MinOf(b) == 0 && context->MaxOf(b) == 1;

  // We expand if exactly one of {a, b} is Boolean. If both are Boolean, it
  // will be presolved into a better version.
  if (a_is_boolean && !b_is_boolean) {
    ExpandIntProdWithBoolean(a, b, p, context);
    ct->Clear();
    context->UpdateRuleStats("int_prod: expanded product with Boolean var");
  } else if (b_is_boolean && !a_is_boolean) {
    ExpandIntProdWithBoolean(b, a, p, context);
    ct->Clear();
    context->UpdateRuleStats("int_prod: expanded product with Boolean var");
  }
}

void ExpandInverse(ConstraintProto* ct, PresolveContext* context) {
  const int size = ct->inverse().f_direct().size();
  CHECK_EQ(size, ct->inverse().f_inverse().size());

  // Make sure the domains are included in [0, size - 1).
  //
  // TODO(user): Add support for UNSAT at expansion. This should create empty
  // domain if UNSAT, so it should still work correctly.
  for (const int ref : ct->inverse().f_direct()) {
    if (!context->IntersectDomainWith(ref, Domain(0, size - 1))) {
      VLOG(1) << "Empty domain for a variable in ExpandInverse()";
      return;
    }
  }
  for (const int ref : ct->inverse().f_inverse()) {
    if (!context->IntersectDomainWith(ref, Domain(0, size - 1))) {
      VLOG(1) << "Empty domain for a variable in ExpandInverse()";
      return;
    }
  }

  // Reduce the domains of each variable by checking that the inverse value
  // exists.
  std::vector<int64> possible_values;
  // Propagate from one vector to its counterpart.
  // Note this reaches the fixpoint as there is a one to one mapping between
  // (variable-value) pairs in each vector.
  const auto filter_inverse_domain = [context, size, &possible_values](
     const google::protobuf::RepeatedField<int>& direct,
     const google::protobuf::RepeatedField<int>& inverse) {
    // Propagate for the inverse vector to the direct vector.
    for (int i = 0; i < size; ++i) {
      possible_values.clear();
      const Domain domain = context->DomainOf(direct[i]);
      bool removed_value = false;
      for (const ClosedInterval& interval : domain) {
        for (int64 j = interval.start; j <= interval.end; ++j) {
          if (context->DomainOf(inverse[j]).Contains(i)) {
            possible_values.push_back(j);
          } else {
            removed_value = true;
          }
        }
      }
      if (removed_value) {
        if (!context->IntersectDomainWith(
                direct[i], Domain::FromValues(possible_values))) {
          VLOG(1) << "Empty domain for a variable in ExpandInverse()";
          return false;
        }
      }
    }
    return true;
  };

  if (!filter_inverse_domain(ct->inverse().f_direct(),
                             ct->inverse().f_inverse())) {
    return;
  }

  if (!filter_inverse_domain(ct->inverse().f_inverse(),
                             ct->inverse().f_direct())) {
    return;
  }

  // Expand the inverse constraint by associating literal to var == value
  // and sharing them between the direct and inverse variables.
  for (int i = 0; i < size; ++i) {
    const int f_i = ct->inverse().f_direct(i);
    const Domain domain = context->DomainOf(f_i);
    for (const ClosedInterval& interval : domain) {
      for (int64 j = interval.start; j <= interval.end; ++j) {
        // We have f[i] == j <=> r[j] == i;
        const int r_j = ct->inverse().f_inverse(j);
        int r_j_i;
        if (context->HasVarValueEncoding(r_j, i, &r_j_i)) {
          context->InsertVarValueEncoding(r_j_i, f_i, j);
        } else {
          const int f_i_j = context->GetOrCreateVarValueEncoding(f_i, j);
          context->InsertVarValueEncoding(f_i_j, r_j, i);
        }
      }
    }
  }

  ct->Clear();
  context->UpdateRuleStats("inverse: expanded");
}

void ExpandElement(ConstraintProto* ct, PresolveContext* context) {
  const ElementConstraintProto& element = ct->element();
  const int index_ref = element.index();
  const int target_ref = element.target();
  const int size = element.vars_size();

  if (!context->IntersectDomainWith(index_ref, Domain(0, size - 1))) {
    VLOG(1) << "Empty domain for the index variable in ExpandElement()";
    return (void)context->NotifyThatModelIsUnsat();
  }

  bool all_constants = true;
  absl::flat_hash_map<int64, int> constant_var_values_usage;
  std::vector<int64> constant_var_values;
  std::vector<int64> invalid_indices;
  Domain index_domain = context->DomainOf(index_ref);
  Domain target_domain = context->DomainOf(target_ref);
  for (const ClosedInterval& interval : index_domain) {
    for (int64 v = interval.start; v <= interval.end; ++v) {
      const int var = element.vars(v);
      const Domain var_domain = context->DomainOf(var);
      if (var_domain.IntersectionWith(target_domain).IsEmpty()) {
        invalid_indices.push_back(v);
        continue;
      }
      if (var_domain.Min() != var_domain.Max()) {
        all_constants = false;
        break;
      }

      const int64 value = var_domain.Min();
      if (constant_var_values_usage[value]++ == 0) {
        constant_var_values.push_back(value);
      }
    }
  }

  if (!invalid_indices.empty() && target_ref != index_ref) {
    if (!context->IntersectDomainWith(
            index_ref, Domain::FromValues(invalid_indices).Complement())) {
      VLOG(1) << "No compatible variable domains in ExpandElement()";
      return (void)context->NotifyThatModelIsUnsat();
    }

    // Re-read the domain.
    index_domain = context->DomainOf(index_ref);
  }

  // This BoolOrs implements the deduction that if all index literals pointing
  // to the same values in the constant array are false, then this value is no
  // no longer valid for the target variable. They are created only for values
  // that have multiples literals supporting them.
  // Order is not important.
  absl::flat_hash_map<int64, BoolArgumentProto*> supports;
  if (all_constants && target_ref != index_ref) {
    if (!context->IntersectDomainWith(
            target_ref, Domain::FromValues(constant_var_values))) {
      VLOG(1) << "Empty domain for the target variable in ExpandElement()";
      return;
    }

    target_domain = context->DomainOf(target_ref);
    if (target_domain.Size() == 1) {
      context->UpdateRuleStats("element: one value array");
      ct->Clear();
      return;
    }

    for (const ClosedInterval& interval : target_domain) {
      for (int64 v = interval.start; v <= interval.end; ++v) {
        const int usage = gtl::FindOrDie(constant_var_values_usage, v);
        if (usage > 1) {
          const int lit = context->GetOrCreateVarValueEncoding(target_ref, v);
          BoolArgumentProto* const support =
              context->working_model->add_constraints()->mutable_bool_or();
          supports[v] = support;
          support->add_literals(NegatedRef(lit));
        }
      }
    }
  }

  // While this is not stricly needed since all value in the index will be
  // covered, it allows to easily detect this fact in the presolve.
  auto* bool_or = context->working_model->add_constraints()->mutable_bool_or();

  for (const ClosedInterval& interval : index_domain) {
    for (int64 v = interval.start; v <= interval.end; ++v) {
      const int var = element.vars(v);
      const int index_lit = context->GetOrCreateVarValueEncoding(index_ref, v);
      const Domain var_domain = context->DomainOf(var);

      bool_or->add_literals(index_lit);

      if (target_ref == index_ref) {
        // This adds extra code. But this information is really important,
        // and hard to retrieve once lost.
        context->AddImplyInDomain(index_lit, var, Domain(v));
      } else if (target_domain.Size() == 1) {
        // TODO(user): If we know all variables are different, then this
        // becomes an equivalence.
        context->AddImplyInDomain(index_lit, var, target_domain);
      } else if (var_domain.Size() == 1) {
        if (all_constants) {
          const int64 value = var_domain.Min();
          if (constant_var_values_usage[value] > 1) {
            // The encoding literal for 'value' of the target_ref has been
            // created before.
            const int target_lit =
                context->GetOrCreateVarValueEncoding(target_ref, value);
            context->AddImplication(index_lit, target_lit);
            gtl::FindOrDie(supports, value)->add_literals(index_lit);
          } else {
            // Try to reuse the literal of the index.
            context->InsertVarValueEncoding(index_lit, target_ref, value);
          }
        } else {
          context->AddImplyInDomain(index_lit, target_ref, var_domain);
        }
      } else {
        ConstraintProto* const ct = context->working_model->add_constraints();
        ct->add_enforcement_literal(index_lit);
        ct->mutable_linear()->add_vars(var);
        ct->mutable_linear()->add_coeffs(1);
        ct->mutable_linear()->add_vars(target_ref);
        ct->mutable_linear()->add_coeffs(-1);
        ct->mutable_linear()->add_domain(0);
        ct->mutable_linear()->add_domain(0);
      }
    }
  }

  if (all_constants) {
    const int64 var_min = target_domain.Min();

    // Scan all values to find the one with the most literals attached.
    int64 most_frequent_value = kint64max;
    int usage = -1;
    for (const auto it : constant_var_values_usage) {
      if (it.second > usage ||
          (it.second == usage && it.first < most_frequent_value)) {
        usage = it.second;
        most_frequent_value = it.first;
      }
    }

    // Add a linear constraint. This helps the linear relaxation.
    //
    // We try to minimize the size of the linear constraint (if the gain is
    // meaningful compared to using the min that has the advantage that all
    // coefficients are positive).
    // TODO(user): Benchmark if using base is always beneficial.
    // TODO(user): Try not to create this if max_usage == 1.
    const int64 base =
        usage > 2 && usage > size / 10 ? most_frequent_value : var_min;
    if (base != var_min) {
      VLOG(3) << "expand element: choose " << base << " with usage " << usage
              << " over " << var_min << " among " << size << " values.";
    }

    LinearConstraintProto* const linear =
        context->working_model->add_constraints()->mutable_linear();
    int64 rhs = -base;
    linear->add_vars(target_ref);
    linear->add_coeffs(-1);
    for (const ClosedInterval& interval : index_domain) {
      for (int64 v = interval.start; v <= interval.end; ++v) {
        const int ref = element.vars(v);
        const int index_lit =
            context->GetOrCreateVarValueEncoding(index_ref, v);
        const int64 delta = context->DomainOf(ref).Min() - base;
        if (RefIsPositive(index_lit)) {
          linear->add_vars(index_lit);
          linear->add_coeffs(delta);
        } else {
          linear->add_vars(NegatedRef(index_lit));
          linear->add_coeffs(-delta);
          rhs -= delta;
        }
      }
    }
    linear->add_domain(rhs);
    linear->add_domain(rhs);

    context->UpdateRuleStats("element: expanded value element");
  } else {
    context->UpdateRuleStats("element: expanded");
  }
  ct->Clear();
}

// Adds clauses so that literals[i] true <=> encoding[value[i]] true.
// This also implicitely use the fact that exactly one alternative is true.
void LinkLiteralsAndValues(
    const std::vector<int>& value_literals, const std::vector<int64>& values,
    const absl::flat_hash_map<int64, int>& target_encoding,
    PresolveContext* context) {
  CHECK_EQ(value_literals.size(), values.size());

  // TODO(user): Make sure this does not appear in the profile.
  // We use a map to make this method deterministic.
  std::map<int, std::vector<int>> value_literals_per_target_literal;

  // If a value is false (i.e not possible), then the tuple with this
  // value is false too (i.e not possible). Conversely, if the tuple is
  // selected, the value must be selected.
  for (int i = 0; i < values.size(); ++i) {
    const int64 v = values[i];
    CHECK(target_encoding.contains(v));
    const int lit = gtl::FindOrDie(target_encoding, v);
    value_literals_per_target_literal[lit].push_back(value_literals[i]);
  }

  // If all tuples supporting a value are false, then this value must be
  // false.
  for (const auto& it : value_literals_per_target_literal) {
    const int target_literal = it.first;
    switch (it.second.size()) {
      case 0: {
        if (!context->SetLiteralToFalse(target_literal)) {
          return;
        }
        break;
      }
      case 1: {
        context->StoreBooleanEqualityRelation(target_literal,
                                              it.second.front());
        break;
      }
      default: {
        BoolArgumentProto* const bool_or =
            context->working_model->add_constraints()->mutable_bool_or();
        bool_or->add_literals(NegatedRef(target_literal));
        for (const int value_literal : it.second) {
          bool_or->add_literals(value_literal);
          context->AddImplication(value_literal, target_literal);
        }
      }
    }
  }
}

void ExpandAutomaton(ConstraintProto* ct, PresolveContext* context) {
  AutomatonConstraintProto& proto = *ct->mutable_automaton();

  if (proto.vars_size() == 0) {
    const int64 initial_state = proto.starting_state();
    for (const int64 final_state : proto.final_states()) {
      if (initial_state == final_state) {
        context->UpdateRuleStats("automaton: empty constraint");
        ct->Clear();
        return;
      }
    }
    // The initial state is not in the final state. The model is unsat.
    return (void)context->NotifyThatModelIsUnsat();
  } else if (proto.transition_label_size() == 0) {
    // Not transitions. The constraint is infeasible.
    return (void)context->NotifyThatModelIsUnsat();
  }

  const int n = proto.vars_size();
  const std::vector<int> vars = {proto.vars().begin(), proto.vars().end()};

  // Compute the set of reachable state at each time point.
  const absl::flat_hash_set<int64> final_states(
      {proto.final_states().begin(), proto.final_states().end()});
  std::vector<absl::flat_hash_set<int64>> reachable_states(n + 1);
  reachable_states[0].insert(proto.starting_state());

  // Forward pass.
  for (int time = 0; time < n; ++time) {
    for (int t = 0; t < proto.transition_tail_size(); ++t) {
      const int64 tail = proto.transition_tail(t);
      const int64 label = proto.transition_label(t);
      const int64 head = proto.transition_head(t);
      if (!reachable_states[time].contains(tail)) continue;
      if (!context->DomainContains(vars[time], label)) continue;
      if (time == n - 1 && !final_states.contains(head)) continue;
      reachable_states[time + 1].insert(head);
    }
  }

  // Backward pass.
  for (int time = n - 1; time >= 0; --time) {
    absl::flat_hash_set<int64> new_set;
    for (int t = 0; t < proto.transition_tail_size(); ++t) {
      const int64 tail = proto.transition_tail(t);
      const int64 label = proto.transition_label(t);
      const int64 head = proto.transition_head(t);

      if (!reachable_states[time].contains(tail)) continue;
      if (!context->DomainContains(vars[time], label)) continue;
      if (!reachable_states[time + 1].contains(head)) continue;
      new_set.insert(tail);
    }
    reachable_states[time].swap(new_set);
  }

  // We will model at each time step the current automaton state using Boolean
  // variables. We will have n+1 time step. At time zero, we start in the
  // initial state, and at time n we should be in one of the final states. We
  // don't need to create Booleans at at time when there is just one possible
  // state (like at time zero).
  absl::flat_hash_map<int64, int> encoding;
  absl::flat_hash_map<int64, int> in_encoding;
  absl::flat_hash_map<int64, int> out_encoding;
  bool removed_values = false;

  for (int time = 0; time < n; ++time) {
    // All these vector have the same size. We will use them to enforce a
    // local table constraint representing one step of the automaton at the
    // given time.
    std::vector<int64> in_states;
    std::vector<int64> transition_values;
    std::vector<int64> out_states;
    for (int i = 0; i < proto.transition_label_size(); ++i) {
      const int64 tail = proto.transition_tail(i);
      const int64 label = proto.transition_label(i);
      const int64 head = proto.transition_head(i);

      if (!reachable_states[time].contains(tail)) continue;
      if (!reachable_states[time + 1].contains(head)) continue;
      if (!context->DomainContains(vars[time], label)) continue;

      // TODO(user): if this transition correspond to just one in-state or
      // one-out state or one variable value, we could reuse the corresponding
      // Boolean variable instead of creating a new one!
      in_states.push_back(tail);
      transition_values.push_back(label);

      // On the last step we don't need to distinguish the output states, so
      // we use zero.
      out_states.push_back(time + 1 == n ? 0 : head);
    }

    std::vector<int> tuple_literals;
    if (transition_values.size() == 1) {
      bool tmp_removed_values = false;
      tuple_literals.push_back(context->GetOrCreateConstantVar(1));
      CHECK_EQ(reachable_states[time + 1].size(), 1);
      if (!context->IntersectDomainWith(vars[time],
                                        Domain(transition_values.front()),
                                        &tmp_removed_values)) {
        return (void)context->NotifyThatModelIsUnsat();
      }
      in_encoding.clear();
      continue;
    } else if (transition_values.size() == 2) {
      const int bool_var = context->NewBoolVar();
      tuple_literals.push_back(bool_var);
      tuple_literals.push_back(NegatedRef(bool_var));
    } else {
      // Note that we do not need the ExactlyOneConstraint(tuple_literals)
      // because it is already implicitely encoded since we have exactly one
      // transition value.
      LinearConstraintProto* const exactly_one =
          context->working_model->add_constraints()->mutable_linear();
      exactly_one->add_domain(1);
      exactly_one->add_domain(1);
      for (int i = 0; i < transition_values.size(); ++i) {
        const int tuple_literal = context->NewBoolVar();
        tuple_literals.push_back(tuple_literal);
        exactly_one->add_vars(tuple_literal);
        exactly_one->add_coeffs(1);
      }
    }

    // Fully encode vars[time].
    {
      std::vector<int64> s = transition_values;
      gtl::STLSortAndRemoveDuplicates(&s);

      encoding.clear();
      if (!context->IntersectDomainWith(vars[time], Domain::FromValues(s),
                                        &removed_values)) {
        return (void)context->NotifyThatModelIsUnsat();
      }

      // Fully encode the variable.
      for (const ClosedInterval& interval : context->DomainOf(vars[time])) {
        for (int64 v = interval.start; v <= interval.end; ++v) {
          encoding[v] = context->GetOrCreateVarValueEncoding(vars[time], v);
        }
      }
    }

    // For each possible out states, create one Boolean variable.
    {
      std::vector<int64> s = out_states;
      gtl::STLSortAndRemoveDuplicates(&s);

      out_encoding.clear();
      if (s.size() == 2) {
        const int var = context->NewBoolVar();
        out_encoding[s.front()] = var;
        out_encoding[s.back()] = NegatedRef(var);
      } else if (s.size() > 2) {
        for (const int64 state : s) {
          out_encoding[state] = context->NewBoolVar();
        }
      }
    }

    if (!in_encoding.empty()) {
      LinkLiteralsAndValues(tuple_literals, in_states, in_encoding, context);
    }
    if (!encoding.empty()) {
      LinkLiteralsAndValues(tuple_literals, transition_values, encoding,
                            context);
    }
    if (!out_encoding.empty()) {
      LinkLiteralsAndValues(tuple_literals, out_states, out_encoding, context);
    }
    in_encoding.swap(out_encoding);
    out_encoding.clear();
  }

  if (removed_values) {
    context->UpdateRuleStats("automaton: reduced variable domains");
  }
  context->UpdateRuleStats("automaton: expanded");
  ct->Clear();
}

void ExpandNegativeTable(ConstraintProto* ct, PresolveContext* context) {
  TableConstraintProto& table = *ct->mutable_table();
  const int num_vars = table.vars_size();
  const int num_original_tuples = table.values_size() / num_vars;
  std::vector<std::vector<int64>> tuples(num_original_tuples);
  int count = 0;
  for (int i = 0; i < num_original_tuples; ++i) {
    for (int j = 0; j < num_vars; ++j) {
      tuples[i].push_back(table.values(count++));
    }
  }

  if (tuples.empty()) {  // Early exit.
    context->UpdateRuleStats("table: empty negated constraint");
    ct->Clear();
    return;
  }

  // Compress tuples.
  const int64 any_value = kint64min;
  std::vector<int64> domain_sizes;
  for (int i = 0; i < num_vars; ++i) {
    domain_sizes.push_back(context->DomainOf(table.vars(i)).Size());
  }
  CompressTuples(domain_sizes, any_value, &tuples);

  // For each tuple, forbid the variables values to be this tuple.
  std::vector<int> clause;
  for (const std::vector<int64>& tuple : tuples) {
    clause.clear();
    for (int i = 0; i < num_vars; ++i) {
      const int64 value = tuple[i];
      if (value == any_value) continue;

      const int literal =
          context->GetOrCreateVarValueEncoding(table.vars(i), value);
      clause.push_back(NegatedRef(literal));
    }
    if (!clause.empty()) {
      BoolArgumentProto* bool_or =
          context->working_model->add_constraints()->mutable_bool_or();
      for (const int lit : clause) {
        bool_or->add_literals(lit);
      }
    }
  }
  context->UpdateRuleStats("table: expanded negated constraint");
  ct->Clear();
}

void ExpandLinMin(ConstraintProto* ct, PresolveContext* context) {
  ConstraintProto* const lin_max = context->working_model->add_constraints();
  for (int i = 0; i < ct->enforcement_literal_size(); ++i) {
    lin_max->add_enforcement_literal(ct->enforcement_literal(i));
  }

  // Target
  SetToNegatedLinearExpression(ct->lin_min().target(),
                               lin_max->mutable_lin_max()->mutable_target());

  for (int i = 0; i < ct->lin_min().exprs_size(); ++i) {
    LinearExpressionProto* const expr = lin_max->mutable_lin_max()->add_exprs();
    SetToNegatedLinearExpression(ct->lin_min().exprs(i), expr);
  }
  ct->Clear();
}

// Add the implications and clauses to link one variable of a table to the
// literals controling if the tuples are possible or not. The parallel vectors
// (tuple_literals, values) contains all valid projected tuples. The
// tuples_with_any vector provides a list of tuple_literals that will support
// any value.
void ProcessOneVariable(const std::vector<int>& tuple_literals,
                        const std::vector<int64>& values, int variable,
                        const std::vector<int>& tuples_with_any,
                        PresolveContext* context) {
  VLOG(2) << "Process var(" << variable << ") with domain "
          << context->DomainOf(variable) << " and " << values.size()
          << " active tuples, and " << tuples_with_any.size() << " any tuples";
  CHECK_EQ(tuple_literals.size(), values.size());
  std::vector<std::pair<int64, int>> pairs;

  // Collect pairs of value-literal.
  for (int i = 0; i < values.size(); ++i) {
    const int64 value = values[i];
    CHECK(context->DomainContains(variable, value));
    pairs.emplace_back(value, tuple_literals[i]);
  }

  // Regroup literal with the same value and add for each the clause: If all the
  // tuples containing a value are false, then this value must be false too.
  std::vector<int> selected;
  std::sort(pairs.begin(), pairs.end());
  for (int i = 0; i < pairs.size();) {
    selected.clear();
    const int64 value = pairs[i].first;
    for (; i < pairs.size() && pairs[i].first == value; ++i) {
      selected.push_back(pairs[i].second);
    }

    CHECK(!selected.empty() || !tuples_with_any.empty());
    if (selected.size() == 1 && tuples_with_any.empty()) {
      context->InsertVarValueEncoding(selected.front(), variable, value);
    } else {
      const int value_literal =
          context->GetOrCreateVarValueEncoding(variable, value);
      BoolArgumentProto* no_support =
          context->working_model->add_constraints()->mutable_bool_or();
      for (const int lit : selected) {
        no_support->add_literals(lit);
        context->AddImplication(lit, value_literal);
      }
      for (const int lit : tuples_with_any) {
        no_support->add_literals(lit);
      }

      // And the "value" literal.
      no_support->add_literals(NegatedRef(value_literal));
    }
  }
}

// Simpler encoding for table constraints with 2 variables.
void AddSizeTwoTable(
    const std::vector<int>& vars, const std::vector<std::vector<int64>>& tuples,
    const std::vector<absl::flat_hash_set<int64>>& values_per_var,
    PresolveContext* context) {
  CHECK_EQ(vars.size(), 2);
  const int left_var = vars[0];
  const int right_var = vars[1];
  if (context->DomainOf(left_var).Size() == 1 ||
      context->DomainOf(right_var).Size() == 1) {
    // A table constraint with at most one variable not fixed is trivially
    // enforced after domain reduction.
    return;
  }

  std::map<int, std::vector<int>> left_to_right;
  std::map<int, std::vector<int>> right_to_left;

  for (const auto& tuple : tuples) {
    const int64 left_value(tuple[0]);
    const int64 right_value(tuple[1]);
    CHECK(context->DomainContains(left_var, left_value));
    CHECK(context->DomainContains(right_var, right_value));

    const int left_literal =
        context->GetOrCreateVarValueEncoding(left_var, left_value);
    const int right_literal =
        context->GetOrCreateVarValueEncoding(right_var, right_value);
    left_to_right[left_literal].push_back(right_literal);
    right_to_left[right_literal].push_back(left_literal);
  }

  int num_implications = 0;
  int num_clause_added = 0;
  int num_large_clause_added = 0;
  auto add_support_constraint =
      [context, &num_clause_added, &num_large_clause_added, &num_implications](
          int lit, const std::vector<int>& support_literals,
          int max_support_size) {
        if (support_literals.size() == max_support_size) return;
        if (support_literals.size() == 1) {
          context->AddImplication(lit, support_literals.front());
          num_implications++;
        } else {
          BoolArgumentProto* bool_or =
              context->working_model->add_constraints()->mutable_bool_or();
          for (const int support_literal : support_literals) {
            bool_or->add_literals(support_literal);
          }
          bool_or->add_literals(NegatedRef(lit));
          num_clause_added++;
          if (support_literals.size() > max_support_size / 2) {
            num_large_clause_added++;
          }
        }
      };

  for (const auto& it : left_to_right) {
    add_support_constraint(it.first, it.second, values_per_var[1].size());
  }
  for (const auto& it : right_to_left) {
    add_support_constraint(it.first, it.second, values_per_var[0].size());
  }
  VLOG(2) << "Table: 2 variables, " << tuples.size() << " tuples encoded using "
          << num_clause_added << " clauses, including "
          << num_large_clause_added << " large clauses, " << num_implications
          << " implications";
}

void ExpandPositiveTable(ConstraintProto* ct, PresolveContext* context) {
  const TableConstraintProto& table = ct->table();
  const std::vector<int> vars(table.vars().begin(), table.vars().end());
  const int num_vars = table.vars_size();
  const int num_original_tuples = table.values_size() / num_vars;

  // Read tuples flat array and recreate the vector of tuples.
  std::vector<std::vector<int64>> tuples(num_original_tuples);
  int count = 0;
  for (int tuple_index = 0; tuple_index < num_original_tuples; ++tuple_index) {
    for (int var_index = 0; var_index < num_vars; ++var_index) {
      tuples[tuple_index].push_back(table.values(count++));
    }
  }

  // Compute the set of possible values for each variable (from the table).
  // Remove invalid tuples along the way.
  std::vector<absl::flat_hash_set<int64>> values_per_var(num_vars);
  int new_size = 0;
  for (int tuple_index = 0; tuple_index < num_original_tuples; ++tuple_index) {
    bool keep = true;
    for (int var_index = 0; var_index < num_vars; ++var_index) {
      const int64 value = tuples[tuple_index][var_index];
      if (!context->DomainContains(vars[var_index], value)) {
        keep = false;
        break;
      }
    }
    if (keep) {
      for (int var_index = 0; var_index < num_vars; ++var_index) {
        values_per_var[var_index].insert(tuples[tuple_index][var_index]);
      }
      std::swap(tuples[tuple_index], tuples[new_size]);
      new_size++;
    }
  }
  tuples.resize(new_size);
  const int num_valid_tuples = tuples.size();

  if (tuples.empty()) {
    context->UpdateRuleStats("table: empty");
    return (void)context->NotifyThatModelIsUnsat();
  }

  // Update variable domains. It is redundant with presolve, but we could be
  // here with presolve = false.
  // Also counts the number of fixed variables.
  int num_fixed_variables = 0;
  for (int var_index = 0; var_index < num_vars; ++var_index) {
    CHECK(context->IntersectDomainWith(
        vars[var_index],
        Domain::FromValues({values_per_var[var_index].begin(),
                            values_per_var[var_index].end()})));
    if (context->DomainOf(vars[var_index]).Size() == 1) {
      num_fixed_variables++;
    }
  }

  if (num_fixed_variables == num_vars - 1) {
    context->UpdateRuleStats("table: one variable not fixed");
    ct->Clear();
    return;
  } else if (num_fixed_variables == num_vars) {
    context->UpdateRuleStats("table: all variables fixed");
    ct->Clear();
    return;
  }

  // Tables with two variables do not need tuple literals.
  if (num_vars == 2) {
    AddSizeTwoTable(vars, tuples, values_per_var, context);
    context->UpdateRuleStats(
        "table: expanded positive constraint with two variables");
    ct->Clear();
    return;
  }

  // It is easier to compute this before compression, as compression will merge
  // tuples.
  int num_prefix_tuples = 0;
  {
    absl::flat_hash_set<absl::Span<const int64>> prefixes;
    for (const std::vector<int64>& tuple : tuples) {
      prefixes.insert(absl::MakeSpan(tuple.data(), num_vars - 1));
    }
    num_prefix_tuples = prefixes.size();
  }

  // TODO(user): reinvestigate ExploreSubsetOfVariablesAndAddNegatedTables.

  // Compress tuples.
  const int64 any_value = kint64min;
  std::vector<int64> domain_sizes;
  for (int i = 0; i < num_vars; ++i) {
    domain_sizes.push_back(values_per_var[i].size());
  }
  CompressTuples(domain_sizes, any_value, &tuples);
  const int num_compressed_tuples = tuples.size();

  if (num_compressed_tuples == 1) {
    // Domains are propagated. We can remove the constraint.
    context->UpdateRuleStats("table: one tuple");
    ct->Clear();
    return;
  }

  // Detect if prefix tuples are all different.
  const bool prefixes_are_all_different = num_prefix_tuples == num_valid_tuples;
  if (prefixes_are_all_different) {
    context->UpdateRuleStats(
        "TODO table: last value implied by previous values");
  }
  // TODO(user): if 2 table constraints share the same valid prefix, the
  // tuple literals can be reused.
  // TODO(user): investigate different encoding for prefix tables. Maybe
  // we can remove the need to create tuple literals.

  // Debug message to log the status of the expansion.
  if (VLOG_IS_ON(2)) {
    // Compute the maximum number of prefix tuples.
    int64 max_num_prefix_tuples = 1;
    for (int var_index = 0; var_index + 1 < num_vars; ++var_index) {
      max_num_prefix_tuples =
          CapProd(max_num_prefix_tuples, values_per_var[var_index].size());
    }

    std::string message =
        absl::StrCat("Table: ", num_vars,
                     " variables, original tuples = ", num_original_tuples);
    if (num_valid_tuples != num_original_tuples) {
      absl::StrAppend(&message, ", valid tuples = ", num_valid_tuples);
    }
    if (prefixes_are_all_different) {
      if (num_prefix_tuples < max_num_prefix_tuples) {
        absl::StrAppend(&message, ", partial prefix = ", num_prefix_tuples, "/",
                        max_num_prefix_tuples);
      } else {
        absl::StrAppend(&message, ", full prefix = true");
      }
    } else {
      absl::StrAppend(&message, ", num prefix tuples = ", num_prefix_tuples);
    }
    if (num_compressed_tuples != num_valid_tuples) {
      absl::StrAppend(&message,
                      ", compressed tuples = ", num_compressed_tuples);
    }
    VLOG(2) << message;
  }

  // Log if we have only two tuples.
  if (num_compressed_tuples == 2) {
    context->UpdateRuleStats("TODO table: two tuples");
  }

  // Create one Boolean variable per tuple to indicate if it can still be
  // selected or not. Note that we don't enforce exactly one tuple to be
  // selected as this is costly.
  //
  // TODO(user): Investigate adding the at_most_one if the number of tuples
  // is small.
  // TODO(user): Investigate it we could recover a linear constraint:
  //       var = sum(tuple_literals[i] * values[i])
  //   It could be done here or along the deductions grouping.
  std::vector<int> tuple_literals(num_compressed_tuples);
  BoolArgumentProto* at_least_one_tuple =
      context->working_model->add_constraints()->mutable_bool_or();

  // If we want to enumerate all solutions, we should not add new variables that
  // can take more than one value for a given feasible solution, otherwise we
  // will have a lot more solution than needed.
  //
  // TODO(user): Alternatively, we could mark those variable so that their
  // value do not count when excluding solution, but we do not have a
  // mecanism for that yet. It might not be easy to track them down when we
  // replace them with equivalent variable too.
  //
  // TODO(user): We use keep_all_feasible_solutions as a proxy for enumerate
  // all solution, but the concept are slightly different though.
  BoolArgumentProto* at_most_one_tuple = nullptr;
  if (context->keep_all_feasible_solutions) {
    at_most_one_tuple =
        context->working_model->add_constraints()->mutable_at_most_one();
  }

  for (int var_index = 0; var_index < num_compressed_tuples; ++var_index) {
    tuple_literals[var_index] = context->NewBoolVar();
    at_least_one_tuple->add_literals(tuple_literals[var_index]);
    if (at_most_one_tuple != nullptr) {
      at_most_one_tuple->add_literals(tuple_literals[var_index]);
    }
  }

  std::vector<int> active_tuple_literals;
  std::vector<int64> active_values;
  std::vector<int> any_tuple_literals;
  for (int var_index = 0; var_index < num_vars; ++var_index) {
    if (values_per_var[var_index].size() == 1) continue;

    active_tuple_literals.clear();
    active_values.clear();
    any_tuple_literals.clear();
    for (int tuple_index = 0; tuple_index < tuple_literals.size();
         ++tuple_index) {
      const int64 value = tuples[tuple_index][var_index];
      const int tuple_literal = tuple_literals[tuple_index];

      if (value == any_value) {
        any_tuple_literals.push_back(tuple_literal);
      } else {
        active_tuple_literals.push_back(tuple_literal);
        active_values.push_back(value);
      }
    }

    if (!active_tuple_literals.empty()) {
      ProcessOneVariable(active_tuple_literals, active_values, vars[var_index],
                         any_tuple_literals, context);
    }
  }

  context->UpdateRuleStats("table: expanded positive constraint");
  ct->Clear();
}
}  // namespace

void ExpandCpModel(PresolveOptions options, PresolveContext* context) {
  if (context->ModelIsUnsat()) return;

  // Make sure all domains are initialized.
  context->InitializeNewDomains();

  const int num_constraints = context->working_model->constraints_size();
  for (int i = 0; i < num_constraints; ++i) {
    ConstraintProto* const ct = context->working_model->mutable_constraints(i);
    bool skip = false;
    switch (ct->constraint_case()) {
      case ConstraintProto::ConstraintCase::kReservoir:
        ExpandReservoir(ct, context);
        break;
      case ConstraintProto::ConstraintCase::kIntMod:
        ExpandIntMod(ct, context);
        break;
      case ConstraintProto::ConstraintCase::kIntProd:
        ExpandIntProd(ct, context);
        break;
      case ConstraintProto::ConstraintCase::kLinMin:
        ExpandLinMin(ct, context);
        break;
      case ConstraintProto::ConstraintCase::kElement:
        if (options.parameters.expand_element_constraints()) {
          ExpandElement(ct, context);
        }
        break;
      case ConstraintProto::ConstraintCase::kInverse:
        ExpandInverse(ct, context);
        break;
      case ConstraintProto::ConstraintCase::kAutomaton:
        if (options.parameters.expand_automaton_constraints()) {
          ExpandAutomaton(ct, context);
        }
        break;
      case ConstraintProto::ConstraintCase::kTable:
        if (ct->table().negated()) {
          ExpandNegativeTable(ct, context);
        } else if (options.parameters.expand_table_constraints()) {
          ExpandPositiveTable(ct, context);
        }
        break;
      default:
        skip = true;
        break;
    }
    if (skip) continue;  // Nothing was done for this constraint.

    // Update variable-contraint graph.
    context->UpdateNewConstraintsVariableUsage();
    if (ct->constraint_case() == ConstraintProto::CONSTRAINT_NOT_SET) {
      context->UpdateConstraintVariableUsage(i);
    }

    // Early exit if the model is unsat.
    if (context->ModelIsUnsat()) return;
  }

  // Make sure the context is consistent.
  context->InitializeNewDomains();

  // Update any changed domain from the context.
  for (int i = 0; i < context->working_model->variables_size(); ++i) {
    FillDomainInProto(context->DomainOf(i),
                      context->working_model->mutable_variables(i));
  }
}

}  // namespace sat
}  // namespace operations_research
