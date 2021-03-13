cmake_minimum_required(VERSION 3.16)
project(nvm_db_benchmark)
set(CMAKE_C_STANDARD 11)

#set(CMAKE_C_COMPILER "/opt/rh/devtoolset-9/root/bin/gcc")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#find_library(libpmemobj REQUIRED)
#find_library(libpmemobj++ REQUIRED PATHS /usr/local/include)
#find_library(memkind REQUIRED PATHS /usr/local/lib/)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${PMEM_LIB_PATH}")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PATH}:${PMEM_LIB_PATH}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(LINK_LIBS pthread pmem pmemobj pmempool numa pmem2)
include_directories(include)
add_definitions(-DLIB_PMEM2)

SET(GCC_COVERAGE_COMPILE_FLAGS "-march=native -pthread -g -O3 -funroll-loops -mmmx -DDO_NOT_VERIFY_RANDOM -DSFENCE_AFTER_STORE")
add_executable(nvm_db_benchmark_fenced main.c benchmark/parsing.c benchmark/pmem_mapping.c benchmark/thread_mapping.c benchmark/bw_benchmark.c benchmark/read_bw_benchmark.c benchmark/write_bw_benchmark.c)
SET(GCC_COVERAGE_COMPILE_FLAGS "-march=native -pthread -g -O3 -funroll-loops -mmmx -DDO_NOT_VERIFY_RANDOM")
add_executable(nvm_db_benchmark main.c benchmark/parsing.c benchmark/pmem_mapping.c benchmark/thread_mapping.c benchmark/bw_benchmark.c benchmark/read_bw_benchmark.c benchmark/write_bw_benchmark.c)

target_link_libraries(nvm_db_benchmark ${LINK_LIBS})
target_link_libraries(nvm_db_benchmark_fenced ${LINK_LIBS})