//
// Copyright 2012 Hakan Kjellerstrand
//
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

using System;
using Google.OrTools.ConstraintSolver;

public class Map2
{
  /**
   *
   * Solves a simple map coloring problem.
   *
   * Alternative version, using a matrix to represent
   * the neighbours.
   *
   * See http://www.hakank.org/google_or_tools/map.py
   *
   *
   */
  private static void Solve()
  {
    Solver solver = new Solver("Map2");

    //
    // data
    //
    int Belgium     = 0;
    int Denmark     = 1;
    int France      = 2;
    int Germany     = 3;
    int Netherlands = 4;
    int Luxembourg  = 5;

    int n = 6;
    int max_num_colors = 4;

    int[,] neighbours =  {{France,     Belgium},
                         {France,     Luxembourg},
                         {France,     Germany},
                         {Luxembourg, Germany},
                         {Luxembourg, Belgium},
                         {Belgium,    Netherlands},
                         {Belgium,    Germany},
                         {Germany,    Netherlands},
                         {Germany,    Denmark}};



    //
    // Decision variables
    //
    IntVar[] color = solver.MakeIntVarArray(n, 1, max_num_colors, "color");

    //
    // Constraints
    //
    for(int i = 0; i < neighbours.GetLength(0); i++) {
      solver.Add(color[neighbours[i,0]] != color[neighbours[i,1]]);
    }

    // Symmetry breaking
    solver.Add(color[Belgium] == 1);


    //
    // Search
    //
    DecisionBuilder db = solver.MakePhase(color,
                                          Solver.CHOOSE_MIN_SIZE_LOWEST_MAX,
                                          Solver.ASSIGN_CENTER_VALUE);

    solver.NewSearch(db);
    while (solver.NextSolution()) {
      Console.Write("colors: ");
      for(int i = 0; i < n; i++) {
        Console.Write("{0} ", color[i].Value());
      }

      Console.WriteLine();
    }

    Console.WriteLine("\nSolutions: {0}", solver.Solutions());
    Console.WriteLine("WallTime: {0}ms", solver.WallTime());
    Console.WriteLine("Failures: {0}", solver.Failures());
    Console.WriteLine("Branches: {0} ", solver.Branches());

    solver.EndSearch();

  }

  public static void Main(String[] args)
  {
    Solve();
  }
}
