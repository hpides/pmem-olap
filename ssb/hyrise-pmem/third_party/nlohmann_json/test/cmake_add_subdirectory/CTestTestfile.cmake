# CMake generated Testfile for 
# Source directory: /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory
# Build directory: /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cmake_add_subdirectory_configure "/usr/local/bin/cmake" "-G" "Unix Makefiles" "-Dnlohmann_json_source=/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json" "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory/project")
set_tests_properties(cmake_add_subdirectory_configure PROPERTIES  FIXTURES_SETUP "cmake_add_subdirectory" _BACKTRACE_TRIPLES "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory/CMakeLists.txt;1;add_test;/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory/CMakeLists.txt;0;")
add_test(cmake_add_subdirectory_build "/usr/local/bin/cmake" "--build" ".")
set_tests_properties(cmake_add_subdirectory_build PROPERTIES  FIXTURES_REQUIRED "cmake_add_subdirectory" _BACKTRACE_TRIPLES "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory/CMakeLists.txt;7;add_test;/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/cmake_add_subdirectory/CMakeLists.txt;0;")