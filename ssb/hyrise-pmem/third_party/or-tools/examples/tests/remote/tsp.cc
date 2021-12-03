// Copyright 2010-2014 Google
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
//
// Traveling Salesman Sample.
//
// This is a sample using the routing library to solve a Traveling Salesman
// Problem.
// The description of the problem can be found here:
// http://en.wikipedia.org/wiki/Travelling_salesman_problem.
// For small problems one can use the hamiltonian path library directly (cf
// graph/hamiltonian_path.h).
// The optimization engine uses local search to improve solutions, first
// solutions being generated using a cheapest addition heuristic.
// Optionally one can randomly forbid a set of random connections between nodes
// (forbidden arcs).

#include "base/unique_ptr.h"

#include "base/callback.h"
#include "base/commandlineflags.h"
#include "base/commandlineflags.h"
#include "base/integral_types.h"
#include "base/join.h"
#include "constraint_solver/routing.h"
#include "base/random.h"

using operations_research::Assignment;
using operations_research::RoutingModel;
using operations_research::ACMRandom;
using operations_research::StrCat;
using operations_research::scoped_ptr;


DEFINE_int32(tsp_size, 10, "Size of Traveling Salesman Problem instance.");
DEFINE_bool(tsp_use_random_matrix, true, "Use random cost matrix.");
DEFINE_int32(tsp_random_forbidden_connections, 0,
             "Number of random forbidden connections.");
DEFINE_bool(tsp_use_deterministic_random_seed, false,
            "Use deterministic random seeds.");
DECLARE_string(routing_first_solution);
DECLARE_bool(routing_no_lns);

// Random seed generator.
int32 GetSeed() {
  if (FLAGS_tsp_use_deterministic_random_seed) {
    return ACMRandom::DeterministicSeed();
  } else {
    return ACMRandom::HostnamePidTimeSeed();
  }
}

// Cost/distance functions.

// Sample function.
int64 MyDistance(RoutingModel::NodeIndex from, RoutingModel::NodeIndex to) {
  // Put your distance code here.
  return (from + to).value();  // for instance
}

// Random matrix.
class RandomMatrix {
 public:
  explicit RandomMatrix(int size) : size_(size) {}
  void Initialize() {
    matrix_.reset(new int64[size_ * size_]);
    const int64 kDistanceMax = 100;
    ACMRandom randomizer(GetSeed());
    for (RoutingModel::NodeIndex from = RoutingModel::kFirstNode; from < size_;
         ++from) {
      for (RoutingModel::NodeIndex to = RoutingModel::kFirstNode; to < size_;
           ++to) {
        if (to != from) {
          matrix_[MatrixIndex(from, to)] = randomizer.Uniform(kDistanceMax);
        } else {
          matrix_[MatrixIndex(from, to)] = 0LL;
        }
      }
    }
  }
  int64 Distance(RoutingModel::NodeIndex from,
                 RoutingModel::NodeIndex to) const {
    return matrix_[MatrixIndex(from, to)];
  }

 private:
  int64 MatrixIndex(RoutingModel::NodeIndex from,
                    RoutingModel::NodeIndex to) const {
    return (from * size_ + to).value();
  }
  std::unique_ptr<int64[]> matrix_;
  const int size_;
};

int main(int argc, char** argv) {
  google::ParseCommandLineFlags( &argc, &argv, true);
  if (FLAGS_tsp_size > 0) {
    // TSP of size FLAGS_tsp_size.
    // Second argument = 1 to build a single tour (it's a TSP).
    // Nodes are indexed from 0 to FLAGS_tsp_size - 1, by default the start of
    // the route is node 0.
    RoutingModel routing(FLAGS_tsp_size, 1);
    // Setting first solution heuristic (cheapest addition).
    FLAGS_routing_first_solution = "PathCheapestArc";
    // Disabling Large Neighborhood Search, comment out to activate it.
    FLAGS_routing_no_lns = true;

    // Setting the cost function.
    // Put a permanent callback to the distance accessor here. The callback
    // has the following signature: ResultCallback2<int64, int64, int64>.
    // The two arguments are the from and to node inidices.
    RandomMatrix matrix(FLAGS_tsp_size);
    if (FLAGS_tsp_use_random_matrix) {
      matrix.Initialize();
      routing.SetArcCostEvaluatorOfAllVehicles(
          NewPermanentCallback(&matrix, &RandomMatrix::Distance));
    } else {
      routing.SetArcCostEvaluatorOfAllVehicles(
          NewPermanentCallback(MyDistance));
    }
    // Forbid node connections (randomly).
    ACMRandom randomizer(GetSeed());
    int64 forbidden_connections = 0;
    while (forbidden_connections < FLAGS_tsp_random_forbidden_connections) {
      const int64 from = randomizer.Uniform(FLAGS_tsp_size - 1);
      const int64 to = randomizer.Uniform(FLAGS_tsp_size - 1) + 1;
      if (routing.NextVar(from)->Contains(to)) {
        LOG(INFO) << "Forbidding connection " << from << " -> " << to;
        routing.NextVar(from)->RemoveValue(to);
        ++forbidden_connections;
      }
    }
    // Solve, returns a solution if any (owned by RoutingModel).
    const Assignment* solution = routing.Solve();
    if (solution != NULL) {
      // Solution cost.
      LOG(INFO) << "Cost " << solution->ObjectiveValue();
      // Inspect solution.
      // Only one route here; otherwise iterate from 0 to routing.vehicles() - 1
      const int route_number = 0;
      std::string route;
      for (int64 node = routing.Start(route_number); !routing.IsEnd(node);
           node = solution->Value(routing.NextVar(node))) {
        route = StrCat(route, StrCat(node, " -> "));
      }
      route = StrCat(route, "0");
      LOG(INFO) << route;
    } else {
      LOG(INFO) << "No solution found.";
    }
  } else {
    LOG(INFO) << "Specify an instance size greater than 0.";
  }
  return 0;
}
