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

import com.google.ortools.linearsolver.MPConstraint;
import com.google.ortools.linearsolver.MPObjective;
import com.google.ortools.linearsolver.MPSolver;
import com.google.ortools.linearsolver.MPVariable;
import com.google.ortools.linearsolver.main_research_linear_solver;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.logging.Logger;


public class TestLinearSolver {
  static {
    System.loadLibrary("jniortools");
    System.setProperty("java.util.logging.SimpleFormatter.format",
        "[%1$tF %1$tT] [%4$-7s] %5$s %n");
  }

  private static final Logger logger = Logger.getLogger(TestLinearSolver.class.getName());

  static void solveAndPrint(MPSolver solver, MPVariable[] variables, MPConstraint[] constraints) {
    logger.info("Number of variables = " + solver.numVariables());
    logger.info("Number of constraints = "+ solver.numConstraints());

    final MPSolver.ResultStatus status = solver.solve();
    // Check that the problem has an optimal solution.
    if (status != MPSolver.ResultStatus.OPTIMAL) {
      logger.severe("The problem does not have an optimal solution!");
    }

    logger.info("Solution:");
    ArrayList<MPVariable> vars = new ArrayList<>(Arrays.asList(variables));
    vars.forEach(
        var -> logger.info(var.name() + " = " + var.solutionValue())
    );
    logger.info("Optimal objective value = " + solver.objective().value());
    logger.info("");
    logger.info("Advanced usage:");
    logger.info("Problem solved in " + solver.wallTime() + " milliseconds");
    logger.info("Problem solved in " + solver.iterations() + " iterations");
    if (solver.isMip()) return;

    vars.forEach(
        var-> logger.info(var.name() + ": reduced cost " + var.reducedCost())
    );

    final double[] activities = solver.computeConstraintActivities();
    ArrayList<MPConstraint> cts = new ArrayList<>(Arrays.asList(constraints));
    cts.forEach(
        ct -> logger.info(ct.name() + ": dual value = " + ct.dualValue()
        + " activity = " + activities[ct.index()])
    );
  }

  static void runLinearProgrammingExample(String problem_type) {
    logger.info("------ Linear programming example with " + problem_type + " ------");

    MPSolver solver = MPSolver.createSolver("LinearProgrammingExample", problem_type);
    if (solver == null) return;

    // x and y are continuous non-negative variables.
    MPVariable x = solver.makeNumVar(0.0, Double.POSITIVE_INFINITY, "x");
    MPVariable y = solver.makeNumVar(0.0, Double.POSITIVE_INFINITY, "y");

    // Objectif function: Maximize 3x + 4y).
    MPObjective objective = solver.objective();
    objective.setCoefficient(x, 3);
    objective.setCoefficient(y, 4);
    objective.setMaximization();

    // x + 2y <= 14.
    final MPConstraint c0 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 14.0, "c0");
    c0.setCoefficient(x, 1);
    c0.setCoefficient(y, 2);

    // 3x - y >= 0.
    final MPConstraint c1 = solver.makeConstraint(0.0, Double.POSITIVE_INFINITY, "c1");
    c1.setCoefficient(x, 3);
    c1.setCoefficient(y, -1);

    // x - y <= 2.
    final MPConstraint c2 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 2.0, "c2");
    c2.setCoefficient(x, 1);
    c2.setCoefficient(y, -1);

    solveAndPrint(solver, new MPVariable[] {x, y}, new MPConstraint[] {c0, c1, c2});
  }

  static void runMixedIntegerProgrammingExample(String problem_type) {
    logger.info("------ Mixed integer programming example with " + problem_type + " ------");

    MPSolver solver =  MPSolver.createSolver("MixedIntegerProgrammingExample", problem_type);
    if (solver == null) return;

    // x and y are continuous non-negative variables.
    MPVariable x = solver.makeIntVar(0.0, Double.POSITIVE_INFINITY, "x");
    MPVariable y = solver.makeIntVar(0.0, Double.POSITIVE_INFINITY, "y");

    // Objectif function: Maximize x + 10 * y.
    MPObjective objective = solver.objective();
    objective.setCoefficient(x, 1);
    objective.setCoefficient(y, 10);
    objective.setMaximization();

    // x + 7 * y <= 17.5.
    final MPConstraint c0 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 17.5, "c0");
    c0.setCoefficient(x, 1);
    c0.setCoefficient(y, 7);

    // x <= 3.5.
    final MPConstraint c1 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 3.5, "c1");
    c1.setCoefficient(x, 1);
    c1.setCoefficient(y, 0);

    solveAndPrint(solver, new MPVariable[] {x, y}, new MPConstraint[] {c0, c1});
  }

  static void runBooleanProgrammingExample(String problem_type) {
    logger.info("------ Boolean programming example with " + problem_type + " ------");

    MPSolver solver =  MPSolver.createSolver("BooleanProgrammingExample", problem_type);
    if (solver == null) return;

    // x and y are continuous non-negative variables.
    MPVariable x = solver.makeBoolVar("x");
    MPVariable y = solver.makeBoolVar("y");

    // Objectif function: Maximize 2 * x + y.
    MPObjective objective = solver.objective();
    objective.setCoefficient(x, 2);
    objective.setCoefficient(y, 1);
    objective.setMinimization();

    // 1 <= x + 2 * y <= 3.
    final MPConstraint c0 = solver.makeConstraint(1, 3, "c0");
    c0.setCoefficient(x, 1);
    c0.setCoefficient(y, 2);

    solveAndPrint(solver, new MPVariable[] {x, y}, new MPConstraint[] {c0});
  }

  static void testSameConstraintName() {
    MPSolver solver = MPSolver.createSolver("testSameConstraintName", "CBC");
    boolean success = true;
    solver.makeConstraint("my_const_name");
    try {
      solver.makeConstraint("my_const_name");
    } catch(Throwable e) {
      System.out.println(e);
      success = false;
    }
    logger.info("Success = " + success);
  }

  static void testSetHintAndSolverGetters() {
    MPSolver solver =  MPSolver.createSolver("testSetHintAndSolverGetters", "GLOP");

    // x and y are continuous non-negative variables.
    MPVariable x = solver.makeIntVar(0.0, Double.POSITIVE_INFINITY, "x");
    MPVariable y = solver.makeIntVar(0.0, Double.POSITIVE_INFINITY, "y");

    // Objectif function: Maximize x + 10 * y.
    MPObjective objective = solver.objective();
    objective.setCoefficient(x, 1);
    objective.setCoefficient(y, 10);
    objective.setMaximization();

    // x + 7 * y <= 17.5.
    final MPConstraint c0 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 17.5, "c0");
    c0.setCoefficient(x, 1);
    c0.setCoefficient(y, 7);

    // x <= 3.5.
    final MPConstraint c1 = solver.makeConstraint(-Double.POSITIVE_INFINITY, 3.5, "c1");
    c1.setCoefficient(x, 1);
    c1.setCoefficient(y, 0);

    if (solver.constraints().length != 2) {
      throw new RuntimeException("WrongConstraintLength");
    }
    if (solver.variables().length != 2) {
      throw new RuntimeException("WrongConstraintLength");
    }

    solver.setHint(new MPVariable[] {x, y}, new double[] {2.0, 3.0});
  }



  public static void main(String[] args) throws Exception {
    testSameConstraintName();
    testSetHintAndSolverGetters();

    runLinearProgrammingExample("GLOP");
    runLinearProgrammingExample("GLPK_LP");
    runLinearProgrammingExample("CLP");
    runLinearProgrammingExample("GUROBI_LP");

    runMixedIntegerProgrammingExample("GLPK");
    runMixedIntegerProgrammingExample("CBC");
    runMixedIntegerProgrammingExample("SCIP");
    runMixedIntegerProgrammingExample("SAT");

    runBooleanProgrammingExample("SAT");
    runBooleanProgrammingExample("BOP");
  }
}
