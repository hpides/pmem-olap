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

#include "ortools/sat/diffn.h"

#include <algorithm>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_join.h"
#include "ortools/base/iterator_adaptors.h"
#include "ortools/base/map_util.h"
#include "ortools/base/stl_util.h"
#include "ortools/sat/cumulative.h"
#include "ortools/sat/disjunctive.h"
#include "ortools/sat/intervals.h"
#include "ortools/sat/sat_solver.h"
#include "ortools/sat/theta_tree.h"
#include "ortools/util/sort.h"

namespace operations_research {
namespace sat {

void AddCumulativeRelaxation(const std::vector<IntervalVariable>& x_intervals,
                             SchedulingConstraintHelper* x,
                             SchedulingConstraintHelper* y, Model* model) {
  auto* integer_trail = model->GetOrCreate<IntegerTrail>();
  std::vector<AffineExpression> sizes;

  int64 min_starts = kint64max;
  int64 max_ends = kint64min;
  for (int box = 0; box < y->NumTasks(); ++box) {
    IntegerVariable s_var = y->DurationVars()[box];
    if (s_var == kNoIntegerVariable || integer_trail->IsFixed(s_var)) {
      sizes.push_back(AffineExpression(y->DurationMin(box)));
    } else {
      sizes.push_back(AffineExpression(s_var));
    }
    min_starts = std::min(min_starts, y->StartMin(box).value());
    max_ends = std::max(max_ends, y->EndMax(box).value());
  }

  const IntegerVariable min_start_var =
      model->Add(NewIntegerVariable(min_starts, max_ends));
  model->Add(IsEqualToMinOf(min_start_var, y->StartVars()));

  const IntegerVariable max_end_var =
      model->Add(NewIntegerVariable(min_starts, max_ends));
  model->Add(IsEqualToMaxOf(max_end_var, y->EndVars()));

  // (max_end - min_start) >= capacity.
  const AffineExpression capacity(
      model->Add(NewIntegerVariable(0, CapSub(max_ends, min_starts))));
  const std::vector<int64> coeffs = {-capacity.coeff.value(), -1, 1};
  model->Add(
      WeightedSumGreaterOrEqual({capacity.var, min_start_var, max_end_var},
                                coeffs, capacity.constant.value()));

  model->Add(Cumulative(x_intervals, sizes, capacity, x));
}

namespace {

// We want for different propagation to reuse as much as possible the same
// line. The idea behind this is to compute the 'canonical' line to use
// when explaining that boxes overlap on the 'y_dim' dimension. We compute
// the multiple of the biggest power of two that is common to all boxes.
IntegerValue FindCanonicalValue(IntegerValue lb, IntegerValue ub) {
  if (lb == ub) return lb;
  if (lb <= 0 && ub > 0) return IntegerValue(0);
  if (lb < 0 && ub <= 0) {
    return -FindCanonicalValue(-ub, -lb);
  }

  int64 mask = 0;
  IntegerValue candidate = ub;
  for (int o = 0; o < 62; ++o) {
    mask = 2 * mask + 1;
    const IntegerValue masked_ub(ub.value() & ~mask);
    if (masked_ub >= lb) {
      candidate = masked_ub;
    } else {
      break;
    }
  }
  return candidate;
}

void SplitDisjointBoxes(const SchedulingConstraintHelper& x,
                        absl::Span<int> boxes,
                        std::vector<absl::Span<int>>* result) {
  result->clear();
  std::sort(boxes.begin(), boxes.end(),
            [&x](int a, int b) { return x.StartMin(a) < x.StartMin(b); });
  int current_start = 0;
  std::size_t current_length = 1;
  IntegerValue current_max_end = x.EndMax(boxes[0]);

  for (int b = 1; b < boxes.size(); ++b) {
    const int box = boxes[b];
    if (x.StartMin(box) < current_max_end) {
      // Merge.
      current_length++;
      current_max_end = std::max(current_max_end, x.EndMax(box));
    } else {
      if (current_length > 1) {  // Ignore lists of size 1.
        result->emplace_back(&boxes[current_start], current_length);
      }
      current_start = b;
      current_length = 1;
      current_max_end = x.EndMax(box);
    }
  }

  // Push last span.
  if (current_length > 1) {
    result->emplace_back(&boxes[current_start], current_length);
  }
}

}  // namespace

#define RETURN_IF_FALSE(f) \
  if (!(f)) return false;

NonOverlappingRectanglesEnergyPropagator::
    ~NonOverlappingRectanglesEnergyPropagator() {}

bool NonOverlappingRectanglesEnergyPropagator::Propagate() {
  const int num_boxes = x_.NumTasks();
  x_.SetTimeDirection(true);
  y_.SetTimeDirection(true);

  active_boxes_.clear();
  cached_areas_.resize(num_boxes);
  cached_dimensions_.resize(num_boxes);
  for (int box = 0; box < num_boxes; ++box) {
    cached_areas_[box] = x_.DurationMin(box) * y_.DurationMin(box);
    if (cached_areas_[box] == 0) continue;

    // TODO(user): Also consider shifted end max.
    Dimension& dimension = cached_dimensions_[box];
    dimension.x_min = x_.ShiftedStartMin(box);
    dimension.x_max = x_.EndMax(box);
    dimension.y_min = y_.ShiftedStartMin(box);
    dimension.y_max = y_.EndMax(box);

    active_boxes_.push_back(box);
  }
  if (active_boxes_.size() <= 1) return true;

  SplitDisjointBoxes(x_, absl::MakeSpan(active_boxes_), &x_split_);
  for (absl::Span<int> x_boxes : x_split_) {
    SplitDisjointBoxes(y_, x_boxes, &y_split_);
    for (absl::Span<int> y_boxes : y_split_) {
      IntegerValue total_sum_of_areas(0);
      for (const int box : y_boxes) {
        total_sum_of_areas += cached_areas_[box];
      }
      for (const int box : y_boxes) {
        RETURN_IF_FALSE(
            FailWhenEnergyIsTooLarge(box, y_boxes, total_sum_of_areas));
      }
    }
  }

  return true;
}

int NonOverlappingRectanglesEnergyPropagator::RegisterWith(
    GenericLiteralWatcher* watcher) {
  const int id = watcher->Register(this);
  x_.WatchAllTasks(id, watcher, /*watch_start_max=*/false,
                   /*watch_end_max=*/true);
  y_.WatchAllTasks(id, watcher, /*watch_start_max=*/false,
                   /*watch_end_max=*/true);
  return id;
}

void NonOverlappingRectanglesEnergyPropagator::SortBoxesIntoNeighbors(
    int box, absl::Span<const int> local_boxes,
    IntegerValue total_sum_of_areas) {
  const Dimension& box_dim = cached_dimensions_[box];

  neighbors_.clear();
  for (const int other_box : local_boxes) {
    if (other_box == box) continue;
    const Dimension& other_dim = cached_dimensions_[other_box];
    const IntegerValue span_x = std::max(box_dim.x_max, other_dim.x_max) -
                                std::min(box_dim.x_min, other_dim.x_min);
    const IntegerValue span_y = std::max(box_dim.y_max, other_dim.y_max) -
                                std::min(box_dim.y_min, other_dim.y_min);
    const IntegerValue bounding_area = span_x * span_y;
    if (bounding_area < total_sum_of_areas) {
      neighbors_.push_back({other_box, bounding_area});
    }
  }
  std::sort(neighbors_.begin(), neighbors_.end());
}

bool NonOverlappingRectanglesEnergyPropagator::FailWhenEnergyIsTooLarge(
    int box, absl::Span<const int> local_boxes,
    IntegerValue total_sum_of_areas) {
  SortBoxesIntoNeighbors(box, local_boxes, total_sum_of_areas);

  Dimension area = cached_dimensions_[box];
  IntegerValue sum_of_areas = cached_areas_[box];

  const auto add_box_energy_in_rectangle_reason = [&](int b) {
    x_.AddEnergyAfterReason(b, x_.DurationMin(b), area.x_min);
    x_.AddEndMaxReason(b, area.x_max);
    y_.AddEnergyAfterReason(b, y_.DurationMin(b), area.y_min);
    y_.AddEndMaxReason(b, area.y_max);
  };

  for (int i = 0; i < neighbors_.size(); ++i) {
    const int other_box = neighbors_[i].box;
    CHECK_GT(cached_areas_[other_box], 0);

    // Update Bounding box.
    area.TakeUnionWith(cached_dimensions_[other_box]);

    // Update sum of areas.
    sum_of_areas += cached_areas_[other_box];
    const IntegerValue bounding_area =
        (area.x_max - area.x_min) * (area.y_max - area.y_min);
    if (bounding_area >= total_sum_of_areas) {
      // Nothing will be deduced. Exiting.
      return true;
    }

    if (sum_of_areas > bounding_area) {
      x_.ClearReason();
      y_.ClearReason();
      add_box_energy_in_rectangle_reason(box);
      for (int j = 0; j <= i; ++j) {
        add_box_energy_in_rectangle_reason(neighbors_[j].box);
      }
      x_.ImportOtherReasons(y_);
      return x_.ReportConflict();
    }
  }
  return true;
}

// Note that x_ and y_ must be initialized with enough intervals when passed
// to the disjunctive propagators.
NonOverlappingRectanglesDisjunctivePropagator::
    NonOverlappingRectanglesDisjunctivePropagator(bool strict,
                                                  SchedulingConstraintHelper* x,
                                                  SchedulingConstraintHelper* y,
                                                  Model* model)
    : global_x_(*x),
      global_y_(*y),
      x_(x->NumTasks(), model),
      y_(y->NumTasks(), model),
      strict_(strict),
      watcher_(model->GetOrCreate<GenericLiteralWatcher>()),
      overload_checker_(&x_),
      forward_detectable_precedences_(true, &x_),
      backward_detectable_precedences_(false, &x_),
      forward_not_last_(true, &x_),
      backward_not_last_(false, &x_),
      forward_edge_finding_(true, &x_),
      backward_edge_finding_(false, &x_) {}

NonOverlappingRectanglesDisjunctivePropagator::
    ~NonOverlappingRectanglesDisjunctivePropagator() {}

void NonOverlappingRectanglesDisjunctivePropagator::Register(
    int fast_priority, int slow_priority) {
  fast_id_ = watcher_->Register(this);
  watcher_->SetPropagatorPriority(fast_id_, fast_priority);
  global_x_.WatchAllTasks(fast_id_, watcher_);
  global_y_.WatchAllTasks(fast_id_, watcher_);

  const int slow_id = watcher_->Register(this);
  watcher_->SetPropagatorPriority(slow_id, slow_priority);
  global_x_.WatchAllTasks(slow_id, watcher_);
  global_y_.WatchAllTasks(slow_id, watcher_);
}

bool NonOverlappingRectanglesDisjunctivePropagator::
    FindBoxesThatMustOverlapAHorizontalLineAndPropagate(
        const SchedulingConstraintHelper& x,
        const SchedulingConstraintHelper& y,
        std::function<bool()> inner_propagate) {
  // Compute relevant events (line in the y dimension).
  active_boxes_.clear();
  events_time_.clear();
  for (int box = 0; box < x.NumTasks(); ++box) {
    if (!strict_ && (x.DurationMin(box) == 0 || y.DurationMin(box) == 0)) {
      continue;
    }

    const IntegerValue start_max = y.StartMax(box);
    const IntegerValue end_min = y.EndMin(box);
    if (start_max < end_min) {
      events_time_.push_back(start_max);
      active_boxes_.push_back(box);
    }
  }

  // Less than 2 boxes, no propagation.
  if (active_boxes_.size() < 2) return true;

  // Add boxes to the event lists they always overlap with.
  gtl::STLSortAndRemoveDuplicates(&events_time_);
  events_overlapping_boxes_.resize(events_time_.size());
  for (int i = 0; i < events_time_.size(); ++i) {
    events_overlapping_boxes_[i].clear();
  }
  for (const int box : active_boxes_) {
    const IntegerValue start_max = y.StartMax(box);
    const IntegerValue end_min = y.EndMin(box);

    for (int i = 0; i < events_time_.size(); ++i) {
      const IntegerValue t = events_time_[i];
      if (t < start_max) continue;
      if (t >= end_min) break;
      events_overlapping_boxes_[i].push_back(box);
    }
  }

  // Scan events chronologically to remove events where there is only one
  // mandatory box, or dominated events lists.
  //
  // Optimization: We do not resize the events_overlapping_boxes_ vector so that
  // we do not free/realloc the memory of the inner vector from one propagate to
  // the next. This save a bit more than 1%.
  int new_size = 0;
  {
    for (std::vector<int>& overlapping_boxes : events_overlapping_boxes_) {
      if (overlapping_boxes.size() < 2) {
        continue;  // Remove current event.
      }
      if (new_size > 0) {
        const std::vector<int>& previous_overlapping_boxes =
            events_overlapping_boxes_[new_size - 1];

        // If the previous set of boxes is included in the current one, replace
        // the old one by the new one.
        //
        // Note that because the events correspond to new boxes, there is no
        // need to check for the other side (current set included in previous
        // set).
        if (std::includes(overlapping_boxes.begin(), overlapping_boxes.end(),
                          previous_overlapping_boxes.begin(),
                          previous_overlapping_boxes.end())) {
          --new_size;
        }
      }

      std::swap(events_overlapping_boxes_[new_size], overlapping_boxes);
      ++new_size;
    }
  }

  // Split lists of boxes into disjoint set of boxes (w.r.t. overlap).
  boxes_to_propagate_.clear();
  reduced_overlapping_boxes_.clear();
  for (int i = 0; i < new_size; ++i) {
    SplitDisjointBoxes(x, absl::MakeSpan(events_overlapping_boxes_[i]),
                       &disjoint_boxes_);
    for (absl::Span<int> sub_boxes : disjoint_boxes_) {
      // Boxes are sorted in a stable manner in the Split method.
      // Note that we do not use reduced_overlapping_boxes_ directly so that
      // the order of iteration is deterministic.
      const auto& insertion = reduced_overlapping_boxes_.insert(sub_boxes);
      if (insertion.second) boxes_to_propagate_.push_back(sub_boxes);
    }
  }

  // And finally propagate.
  // TODO(user): Sorting of boxes seems influential on the performance. Test.
  for (const absl::Span<const int> boxes : boxes_to_propagate_) {
    x_.ResetFromSubset(x, boxes);
    y_.ResetFromSubset(y, boxes);

    // Collect the common overlapping coordinates of all boxes.
    IntegerValue lb(kint64min);
    IntegerValue ub(kint64max);
    for (int i = 0; i < y_.NumTasks(); ++i) {
      lb = std::max(lb, y_.StartMax(i));
      ub = std::min(ub, y_.EndMin(i) - 1);
    }
    CHECK_LE(lb, ub);

    // TODO(user): We should scan the integer trail to find the oldest
    // non-empty common interval. Then we can pick the canonical value within
    // it.

    // We want for different propagation to reuse as much as possible the same
    // line. The idea behind this is to compute the 'canonical' line to use
    // when explaining that boxes overlap on the 'y_dim' dimension. We compute
    // the multiple of the biggest power of two that is common to all boxes.
    const IntegerValue line_to_use_for_reason = FindCanonicalValue(lb, ub);

    // Setup x_dim for propagation.
    x_.SetOtherHelper(&y_, line_to_use_for_reason);

    RETURN_IF_FALSE(inner_propagate());
  }

  return true;
}

bool NonOverlappingRectanglesDisjunctivePropagator::Propagate() {
  global_x_.SetTimeDirection(true);
  global_y_.SetTimeDirection(true);

  std::function<bool()> inner_propagate;
  if (watcher_->GetCurrentId() == fast_id_) {
    inner_propagate = [this]() {
      if (x_.NumTasks() == 2) {
        // In that case, we can use simpler algorithms.
        // Note that this case happens frequently (~30% of all calls to this
        // method according to our tests).
        RETURN_IF_FALSE(PropagateTwoBoxes());
      } else {
        RETURN_IF_FALSE(overload_checker_.Propagate());
        RETURN_IF_FALSE(forward_detectable_precedences_.Propagate());
        RETURN_IF_FALSE(backward_detectable_precedences_.Propagate());
      }
      return true;
    };
  } else {
    inner_propagate = [this]() {
      if (x_.NumTasks() <= 2) return true;
      RETURN_IF_FALSE(forward_not_last_.Propagate());
      RETURN_IF_FALSE(backward_not_last_.Propagate());
      RETURN_IF_FALSE(backward_edge_finding_.Propagate());
      RETURN_IF_FALSE(forward_edge_finding_.Propagate());
      return true;
    };
  }

  RETURN_IF_FALSE(FindBoxesThatMustOverlapAHorizontalLineAndPropagate(
      global_x_, global_y_, inner_propagate));

  // We can actually swap dimensions to propagate vertically.
  RETURN_IF_FALSE(FindBoxesThatMustOverlapAHorizontalLineAndPropagate(
      global_y_, global_x_, inner_propagate));

  return true;
}

// Specialized propagation on only two boxes that must intersect with the
// given y_line_for_reason.
bool NonOverlappingRectanglesDisjunctivePropagator::PropagateTwoBoxes() {
  // For each direction and each order, we test if the boxes can be disjoint.
  const int state =
      (x_.EndMin(0) <= x_.StartMax(1)) + 2 * (x_.EndMin(1) <= x_.StartMax(0));

  const auto left_box_before_right_box = [this](int left, int right) {
    // left box pushes right box.
    const IntegerValue left_end_min = x_.EndMin(left);
    if (left_end_min > x_.StartMin(right)) {
      x_.ClearReason();
      x_.AddReasonForBeingBefore(left, right);
      x_.AddEndMinReason(left, left_end_min);
      RETURN_IF_FALSE(x_.IncreaseStartMin(right, left_end_min));
    }

    // right box pushes left box.
    const IntegerValue right_start_max = x_.StartMax(right);
    if (right_start_max < x_.EndMax(left)) {
      x_.ClearReason();
      x_.AddReasonForBeingBefore(left, right);
      x_.AddStartMaxReason(right, right_start_max);
      RETURN_IF_FALSE(x_.DecreaseEndMax(left, right_start_max));
    }

    return true;
  };

  switch (state) {
    case 0: {  // Conflict.
      x_.ClearReason();
      x_.AddReasonForBeingBefore(0, 1);
      x_.AddReasonForBeingBefore(1, 0);
      return x_.ReportConflict();
    }
    case 1: {  // b1 is left of b2.
      return left_box_before_right_box(0, 1);
    }
    case 2: {  // b2 is left of b1.
      return left_box_before_right_box(1, 0);
    }
    default: {  // Nothing to deduce.
      return true;
    }
  }
}

#undef RETURN_IF_FALSE
}  // namespace sat
}  // namespace operations_research
