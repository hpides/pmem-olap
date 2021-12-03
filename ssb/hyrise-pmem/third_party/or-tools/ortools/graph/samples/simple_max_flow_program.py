# Copyright 2010-2018 Google LLC
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# [START program]
"""From Taha 'Introduction to Operations Research', example 6.4-2."""
# [START import]
from __future__ import print_function
from ortools.graph import pywrapgraph
# [END import]


def main():
    """MaxFlow simple interface example."""

    # [START data]
    # Define three parallel arrays: start_nodes, end_nodes, and the capacities
    # between each pair. For instance, the arc from node 0 to node 1 has a
    # capacity of 20.

    start_nodes = [0, 0, 0, 1, 1, 2, 2, 3, 3]
    end_nodes = [1, 2, 3, 2, 4, 3, 4, 2, 4]
    capacities = [20, 30, 10, 40, 30, 10, 20, 5, 20]
    # [END data]

    # Instantiate a SimpleMaxFlow solver.
    # [START constraints]
    max_flow = pywrapgraph.SimpleMaxFlow()
    # Add each arc.
    for arc in zip(start_nodes, end_nodes, capacities):
        max_flow.AddArcWithCapacity(arc[0], arc[1], arc[2])
    # [END constraints]

    # [START solve]
    # Find the maximum flow between node 0 and node 4.
    if max_flow.Solve(0, 4) == max_flow.OPTIMAL:
        print('Max flow:', max_flow.OptimalFlow())
        print('')
        print('  Arc    Flow / Capacity')
        for i in range(max_flow.NumArcs()):
            print('%1s -> %1s   %3s  / %3s' %
                  (max_flow.Tail(i), max_flow.Head(i), max_flow.Flow(i),
                   max_flow.Capacity(i)))
        print('Source side min-cut:', max_flow.GetSourceSideMinCut())
        print('Sink side min-cut:', max_flow.GetSinkSideMinCut())
    else:
        print('There was an issue with the max flow input.')
    # [END solve]


if __name__ == '__main__':
    main()
# [END program]
