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

#include "ortools/sat/util.h"

#include <algorithm>
#include <cmath>

#include "ortools/base/stl_util.h"

namespace operations_research {
namespace sat {

int MoveOneUnprocessedLiteralLast(const std::set<LiteralIndex>& processed,
                                  int relevant_prefix_size,
                                  std::vector<Literal>* literals) {
  if (literals->empty()) return -1;
  if (!gtl::ContainsKey(processed, literals->back().Index())) {
    return std::min<int>(relevant_prefix_size, literals->size());
  }

  // To get O(n log n) size of suffixes, we will first process the last n/2
  // literals, we then move all of them first and process the n/2 literals left.
  // We use the same algorithm recursively. The sum of the suffixes' size S(n)
  // is thus S(n/2) + n + S(n/2). That gives us the correct complexity. The code
  // below simulates one step of this algorithm and is made to be "robust" when
  // from one call to the next, some literals have been removed (but the order
  // of literals is preserved).
  int num_processed = 0;
  int num_not_processed = 0;
  int target_prefix_size = literals->size() - 1;
  for (int i = literals->size() - 1; i >= 0; i--) {
    if (gtl::ContainsKey(processed, (*literals)[i].Index())) {
      ++num_processed;
    } else {
      ++num_not_processed;
      target_prefix_size = i;
    }
    if (num_not_processed >= num_processed) break;
  }
  if (num_not_processed == 0) return -1;
  target_prefix_size = std::min(target_prefix_size, relevant_prefix_size);

  // Once a prefix size has been decided, it is always better to
  // enqueue the literal already processed first.
  std::stable_partition(literals->begin() + target_prefix_size, literals->end(),
                        [&processed](Literal l) {
                          return gtl::ContainsKey(processed, l.Index());
                        });
  return target_prefix_size;
}

void IncrementalAverage::Reset(double reset_value) {
  num_records_ = 0;
  average_ = reset_value;
}

void IncrementalAverage::AddData(double new_record) {
  num_records_++;
  average_ += (new_record - average_) / num_records_;
}

void ExponentialMovingAverage::AddData(double new_record) {
  num_records_++;
  average_ = (num_records_ == 1)
                 ? new_record
                 : (new_record + decaying_factor_ * (average_ - new_record));
}

void Percentile::AddRecord(double record) {
  records_.push_front(record);
  if (records_.size() > record_limit_) {
    records_.pop_back();
  }
}

double Percentile::GetPercentile(double percent) {
  CHECK_GT(records_.size(), 0);
  CHECK_LE(percent, 100.0);
  CHECK_GE(percent, 0.0);
  std::vector<double> sorted_records(records_.begin(), records_.end());
  std::sort(sorted_records.begin(), sorted_records.end());
  const int num_records = sorted_records.size();

  const double percentile_rank =
      static_cast<double>(num_records) * percent / 100.0 - 0.5;
  if (percentile_rank <= 0) {
    return sorted_records.front();
  } else if (percentile_rank >= num_records - 1) {
    return sorted_records.back();
  }
  // Interpolate.
  DCHECK_GE(num_records, 2);
  DCHECK_LT(percentile_rank, num_records - 1);
  const int lower_rank = static_cast<int>(std::floor(percentile_rank));
  DCHECK_LT(lower_rank, num_records - 1);
  return sorted_records[lower_rank] +
         (percentile_rank - lower_rank) *
             (sorted_records[lower_rank + 1] - sorted_records[lower_rank]);
}

void CompressTuples(absl::Span<const int64> domain_sizes, int64 any_value,
                    std::vector<std::vector<int64>>* tuples) {
  if (tuples->empty()) return;

  // Remove duplicates if any.
  gtl::STLSortAndRemoveDuplicates(tuples);

  const int num_vars = (*tuples)[0].size();

  std::vector<int> to_remove;
  std::vector<int64> tuple_minus_var_i(num_vars - 1);
  for (int i = 0; i < num_vars; ++i) {
    const int domain_size = domain_sizes[i];
    if (domain_size == 1) continue;
    absl::flat_hash_map<const std::vector<int64>, std::vector<int>>
        masked_tuples_to_indices;
    for (int t = 0; t < tuples->size(); ++t) {
      int out = 0;
      for (int j = 0; j < num_vars; ++j) {
        if (i == j) continue;
        tuple_minus_var_i[out++] = (*tuples)[t][j];
      }
      masked_tuples_to_indices[tuple_minus_var_i].push_back(t);
    }
    to_remove.clear();
    for (const auto& it : masked_tuples_to_indices) {
      if (it.second.size() != domain_size) continue;
      (*tuples)[it.second.front()][i] = any_value;
      to_remove.insert(to_remove.end(), it.second.begin() + 1, it.second.end());
    }
    std::sort(to_remove.begin(), to_remove.end(), std::greater<int>());
    for (const int t : to_remove) {
      (*tuples)[t] = tuples->back();
      tuples->pop_back();
    }
  }
}

}  // namespace sat
}  // namespace operations_research
