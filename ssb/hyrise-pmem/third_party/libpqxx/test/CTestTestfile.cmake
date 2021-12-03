# CMake generated Testfile for 
# Source directory: /hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/test
# Build directory: /hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(runner "/hpi/fs00/home/lars.bollmeier/hyrise/runner")
set_tests_properties(runner PROPERTIES  WORKING_DIRECTORY "/hpi/fs00/home/lars.bollmeier/hyrise" _BACKTRACE_TRIPLES "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/test/CMakeLists.txt;78;add_test;/hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/test/CMakeLists.txt;0;")
subdirs("unit")
