// Copyright 2011 Hakan Kjellerstrand hakank@gmail.com
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
package com.google.ortools.contrib;

import com.google.ortools.constraintsolver.DecisionBuilder;
import com.google.ortools.constraintsolver.IntVar;
import com.google.ortools.constraintsolver.Solver;
import java.io.*;
import java.text.*;
import java.util.*;

public class NQueens2 {

  static {
    System.loadLibrary("jniortools");
  }

  /** Solves the N Queens problem. See http://www.hakank.org/google_or_tools/nqueens2.py */
  private static void solve(int n, int num, int print) {

    Solver solver = new Solver("NQueens");

    System.out.println("n: " + n);

    //
    // variables
    //
    IntVar[] q = solver.makeIntVarArray(n, 0, n - 1, "q");

    //
    // constraints
    //
    solver.addConstraint(solver.makeAllDifferent(q));

    IntVar[] q1 = new IntVar[n];
    IntVar[] q2 = new IntVar[n];
    for (int i = 0; i < n; i++) {
      q1[i] = solver.makeSum(q[i], i).var();
      q2[i] = solver.makeSum(q[i], -i).var();
    }
    solver.addConstraint(solver.makeAllDifferent(q1));
    solver.addConstraint(solver.makeAllDifferent(q2));

    //
    // Solve
    //
    DecisionBuilder db =
        solver.makePhase(q, solver.CHOOSE_MIN_SIZE_LOWEST_MAX, solver.ASSIGN_CENTER_VALUE);
    solver.newSearch(db);
    int c = 0;
    while (solver.nextSolution()) {
      if (print != 0) {
        for (int i = 0; i < n; i++) {
          System.out.print(q[i].value() + " ");
        }
        System.out.println();
      }
      c++;
      if (num > 0 && c >= num) {
        break;
      }
    }
    solver.endSearch();

    // Statistics
    System.out.println();
    System.out.println("Solutions: " + solver.solutions());
    System.out.println("Failures: " + solver.failures());
    System.out.println("Branches: " + solver.branches());
    System.out.println("Wall time: " + solver.wallTime() + "ms");
  }

  public static void main(String[] args) throws Exception {
    int n = 8;
    int num = 0;
    int print = 1;

    if (args.length > 0) {
      n = Integer.parseInt(args[0]);
    }

    if (args.length > 1) {
      num = Integer.parseInt(args[1]);
    }

    if (args.length > 2) {
      print = Integer.parseInt(args[2]);
    }

    NQueens2.solve(n, num, print);
  }
}
