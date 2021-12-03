cmake_minimum_required(VERSION 3.16)
project(nvm_db_benchmark)
set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

MESSAGE(${CMAKE_MODULE_PATH})
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${PMEM_LIB_PATH}")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PATH}:${PMEM_LIB_PATH}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(Libpmem REQUIRED)
find_package(Libpmem2 REQUIRED)

if (LIBPMEM_FOUND AND LIBPMEM2_FOUND)
    find_library(pmem "${LIBPMEM_LIBRARY}")
    SET(LINK_LIBS pthread numa  "${LIBPMEM_LIBRARY}" "${LIBPMEM2_LIBRARY}")
    include_directories(include "${LIBPMEM_INCLUDE_DIR}" "${LIBPMEM2_INCLUDE_DIR}")
    add_definitions(-DLIB_PMEM2)

    SET(GCC_COVERAGE_COMPILE_FLAGS "-static -march=native -pthread -g -O3 -funroll-loops -mmmx -DDO_NOT_VERIFY_RANDOM -DSFENCE_AFTER_STORE=TRUE")
    add_executable(nvm_db_benchmark main.c benchmark/parsing.c benchmark/pmem_mapping.c benchmark/thread_mapping.c benchmark/bw_benchmark.c benchmark/read_bw_benchmark.c benchmark/write_bw_benchmark.c)
    add_executable(nvm_db_benchmark_fenced main.c benchmark/parsing.c benchmark/pmem_mapping.c benchmark/thread_mapping.c benchmark/bw_benchmark.c benchmark/read_bw_benchmark.c benchmark/write_bw_benchmark.c)
    
    target_compile_definitions(nvm_db_benchmark_fenced PUBLIC SFENCE_AFTER_STORE)
    target_link_libraries(nvm_db_benchmark "${LINK_LIBS}")
    target_link_libraries(nvm_db_benchmark_fenced "${LINK_LIBS}")
endif()
