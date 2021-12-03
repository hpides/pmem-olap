# Copyright 2013 Google Inc. All Rights Reserved.
#
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

cmake_minimum_required(VERSION 3.1)

project(cpp_btree VERSION 2.0.0 LANGUAGES CXX)

include(ExternalProject)

option(builds_tests "Build the unit tests is enabled." OFF)
option(install "Install CMake targets during install step." ON)

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "-Wall -g -O2")

include(GNUInstallDirs)

set(BTREE_INCLUDE_INSTALL_DIR       "${CMAKE_INSTALL_INCLUDEDIR}")
set(BTREE_INCLUDE_BUILD_DIR         "${PROJECT_SOURCE_DIR}/include/")

if(build_tests)
    enable_testing()
    include_directories($ENV{GTEST_ROOT}/include)
    link_directories($ENV{GTEST_ROOT})
    include_directories($ENV{GFLAGS_ROOT}/include)
    link_directories($ENV{GFLAGS_ROOT}/lib)
    include_directories("${BTREE_INCLUDE_BUILD_DIR}")
    add_executable(btree_test test/src/btree_test.cc test/src/btree_test_flags.cc)
    add_executable(safe_btree_test test/src/safe_btree_test.cc test/src/btree_test_flags.cc)
    add_executable(btree_bench test/src/btree_bench.cc test/src/btree_test_flags.cc)
    target_link_libraries(btree_test gtest_main gtest gflags pthread)
    target_link_libraries(safe_btree_test gtest_main gtest gflags pthread)
    target_link_libraries(btree_bench gflags gtest pthread)
endif()

if(install)
    install(
        DIRECTORY ${BTREE_INCLUDE_BUILD_DIR}
        DESTINATION ${BTREE_INCLUDE_INSTALL_DIR}
    )
endif()