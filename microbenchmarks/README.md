# Benchmarking Source Code

This directory contains the source code for the performed microbenchmarks.

## Building


```sh
mkdir build
cd build
cmake ..
cmake --build .
```

This will give you a `nvm_db_benchmark` and `nvm_db_benchmark_fenced` binary which then can be used in the benchmarking scripts.

## Usage

The following options can be specified when running the benchmark binary:

Option | Possible Values | Description
-------|-----------------|------------
read | nt, mov, simd | Specify a read benchmark and its used mode. All subsequent options further specify that benchmark until the next read/write option is given.
write | nt, clwb, store, stride | Specify a write benchmark and its used mode. All subsequent options further specify that benchmark until the next read/write option is given.
threads | Number | Specify the number of threads used
ad | Number (Bytes) | Specify the number of bytes each thread reads during a cycle 
mapping_file | Unix File Path | Specify the path to a specific NUMA mapping. See the `mapping_files` folder for examples
package | Unix File Path | Specify the path to the PMEM mapping this benchmark will be run on
verbose | None | Enable verbose output
random | None | Use random access
measure_group | Number | Allows to group multiple read/write benchmarks with the same measure_group ID into one output statistic 
dram | None | Run benchmarks on DRAM
numa | Comma separated numbers | Specify the NUMA nodes used
reuse_random | None | Enable the reuse of random access addresses 
access_pattern | disjoint, log | Specify the access pattern
align | None | Align write   

Please see the `scripts` directory for example usage.

