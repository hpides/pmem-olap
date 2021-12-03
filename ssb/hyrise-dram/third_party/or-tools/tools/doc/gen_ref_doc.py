"""Generate OR-Tools ref doc using Doxygen."""

from __future__ import print_function

import re
import os


def main(version_number):
  """For each doc section, edit the doxy and header files, and generate the doc.
  """
  sections = create_section_data()
  doxy_tmp = 'tools/doc/tmp.doxy'
  header_tmp = 'tools/doc/header.tmp.html'

  for section in sections:
    doxyfile = 'tools/doc/' + section['doxyfile']
    headerfile = 'tools/doc/' + section['headerfile']
    output_dir = section['output_dir']
    project_name = section['project name']
    input_files = section['input_files']
    # Edit doxyfile.
    project_name_string = 'PROJECT_NAME = ' + project_name
    project_number_string = 'PROJECT_NUMBER = ' + version_number
    html_output_string = 'HTML_OUTPUT = ' + output_dir
    input_string = 'INPUT = ' + input_files
    f = open(doxyfile, 'r')
    g = open(doxy_tmp, 'w')
    filedata = f.read()
    filedata = re.sub('PROJECT_NAME', project_name_string, filedata)
    filedata = re.sub('PROJECT_NUMBER', project_number_string, filedata)
    filedata = re.sub('HTML_OUTPUT', html_output_string, filedata)
    if input_files:
      filedata = re.sub('INPUT', input_string, filedata)

    # Write filedata.
    g.write(filedata)
    f.close()
    g.close()

    # Edit header file.
    title = section['title']

    f = open(headerfile, 'r')
    g = open(header_tmp, 'w')
    filedata = f.read()
    filedata = re.sub('Banner Text', 'Google OR-Tools ' + version_number, filedata)
    filedata = re.sub('Page Title', title, filedata)
    # Write filedata.
    g.write(filedata)
    f.close()
    g.close()

    # Generate the doc.
    os.system('doxygen tools/doc/tmp.doxy')
  # Remove temp files.
  os.system('rm tools/doc/tmp.doxy')
  os.system('rm tools/doc/header.tmp.html')

def create_section_data():
  sections = [
    {
      'output_dir': 'cpp_algorithms',
      'project name': 'Algorithms',
      'title': 'C++ Reference: Algorithms',
      'doxyfile': 'cpp.doxy.in',
      'headerfile': 'cpp.header.html.in',
      'input_files': 'ortools/algorithms/dense_doubly_linked_list.h ' +
      'ortools/algorithms/dynamic_partition.h ' +
      'ortools/algorithms/dynamic_permutation.h ' +
      'ortools/algorithms/find_graph_symmetries.h ' +
      'ortools/algorithms/hungarian.h ' +
      'ortools/algorithms/knapsack_solver.h ' +
      'ortools/algorithms/sparse_permutation.h'
    },
    {
      'output_dir': 'cpp_sat',
      'project name': 'CP-SAT',
      'title': 'C++ Reference: CP-SAT',
      'doxyfile': 'cpp.doxy.in',
      'headerfile': 'cpp.header.html.in',
      'input_files':
      'ortools/sat/cp_model.h ' +
      'ortools/sat/cp_model_solver.h ' +
      'ortools/sat/model.h ' +
      'ortools/util/sorted_interval_list.h ' +
      'ortools/util/time_limit.h ' +
      'ortools/gen/ortools/sat/boolean_problem.pb.h ' +
      'ortools/gen/ortools/sat/cp_model.pb.h ' +
      'ortools/gen/ortools/sat/sat_parameters.pb.h'
    },
    {
      'output_dir': 'cpp_graph',
      'project name': 'Graph',
      'title': 'C++ Reference: Graph',
      'doxyfile': 'cpp.doxy.in',
      'headerfile': 'cpp.header.html.in',
      'input_files':
      'ortools/graph/christofides.h ' +
      'ortools/graph/cliques.h ' +
      'ortools/graph/connected_components.h ' +
      'ortools/graph/connectivity.h ' +
      'ortools/graph/ebert_graph.h ' +
      'ortools/graph/eulerian_path.h ' +
      'ortools/graph/graph.h ' +
      'ortools/graph/graphs.h ' +
      'ortools/graph/hamiltonian_path.h ' +
      'ortools/graph/io.h ' +
      'ortools/graph/iterators.h ' +
      'ortools/graph/linear_assignment.h ' +
      'ortools/graph/max_flow.h ' +
      'ortools/graph/min_cost_flow.h ' +
      'ortools/graph/minimum_spanning_tree.h ' +
      'ortools/graph/one_tree_lower_bound.h ' +
      'ortools/graph/shortestpaths.h ' +
      'ortools/graph/strongly_connected_components.h ' +
      'ortools/graph/util.h ' +
      'ortools/gen/ortools/graph/flow_problem.pb.h '
    },
    {
      'output_dir': 'cpp_linear',
      'project name': 'Linear solver',
      'title': 'C++ Reference: Linear solver',
      'doxyfile': 'cpp.doxy.in',
      'headerfile': 'cpp.header.html.in',
      'input_files':
      'ortools/linear_solver/linear_expr.h ' +
      'ortools/linear_solver/linear_solver.h ' +
      'ortools/linear_solver/model_exporter.h ' +
      'ortools/linear_solver/model_exporter_swig_helper.h ' +
      'ortools/linear_solver/model_validator.h ' +
      'ortools/gen/ortools/linear_solver/linear_solver.pb.h '
    },
    {
      'output_dir': 'cpp_routing',
      'project name': 'Routing',
      'title': 'C++ Reference: Routing',
      'doxyfile': 'cpp.doxy.in',
      'headerfile': 'cpp.header.html.in',
      'input_files':
      'ortools/constraint_solver/constraint_solver.h ' +
      'ortools/constraint_solver/constraint_solveri.h ' +
      'ortools/constraint_solver/routing.h ' +
      'ortools/constraint_solver/routing_flags.h ' +
      'ortools/constraint_solver/routing_index_manager.h ' +
      'ortools/constraint_solver/routing_lp_scheduling.h ' +
      'ortools/constraint_solver/routing_neighborhoods.h ' +
      'ortools/constraint_solver/routing_parameters.h ' +
      'ortools/constraint_solver/routing_types.h ' +
      'ortools/gen/ortools/constraint_solver/assignment.pb.h ' +
      'ortools/gen/ortools/constraint_solver/demon_profiler.pb.h ' +
      'ortools/gen/ortools/constraint_solver/routing_enums.pb.h ' +
      'ortools/gen/ortools/constraint_solver/routing_parameters.pb.h ' +
      'ortools/gen/ortools/constraint_solver/search_limit.pb.h ' +
      'ortools/gen/ortools/constraint_solver/solver_parameters.pb.h '
    },
    {
      'output_dir': 'dotnet',
      'project name': 'OR-Tools',
      'title': 'DotNet Reference',
      'doxyfile': 'dotnet.doxy.in',
      'headerfile': 'dotnet.header.html.in',
      'input_files': ''
    },
    {
      'output_dir': 'java',
      'project name': 'OR-Tools',
      'title': 'Java Reference',
      'doxyfile': 'java.doxy.in',
      'headerfile': 'java.header.html.in',
      'input_files': ''
    }
  ]
  return sections

if __name__ == '__main__':
  version_number = '7.7'
  main(version_number)
