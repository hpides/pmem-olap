# Star Schema Benchmark(SSB)

In this directory are all source files used to produce the figure 13 and 14 in the last chapter of our paper.

For this, we provide
- a handcrafted version of our ssb (./handcrafted)
- the in-memory database hyrise, both in an unmodified version (hyrise-dram) as well as in a version that uses PMEM for all table and intermediate allocations  (hyrise-pmem)
- tables were generated with a generator (./ssb-dbgen, taken from [GitHub](https://github.com/electrum/ssb-dbgen)



## Tables
**NOTICE: Generating the necessary table data WILL take a long time and requires some none-trivial adjustments. E.g., for our measurements, we converted the generated .tbl files into .csv and adjusted the columns to a fixed length according to the indices listed in handcrafted/common.hpp. We recommend using the files provided on our ftp.**

```sh
sftp <link provided in email>
get -r pmem-olap-reproducability-tables

```

There are two folders in the tables directory:
- **hyrise** contains tables with a scaling factor of 50, these can be left in place.
- **handcrafted** contains tables with a scaling factor of 100, these must be copied to both packages of you PMEM for the multi-socket benchmarks.


To generate the tables, run:
```sh
cd ssb-dbgen
make
./dbgen -s 100 #or 100 for the hyrise benchmarks
cd ..
mkdir tables && cp ssb-dbgen/*.tbl tables/

```


## Hyrise
### Building
To use Hyrise, both the pmem-, as well as the dram-version have to be build.
For the requirements, please run `install_dependencies.sh` and/or consider the DEPENDENCIES.md
In addition, you will need:
```
libpmem
libpmem2
libmemkind
boost_1_73_0 
```
The older version of boost is needed as in newer versions some functions have been moved, which breaks this outdated version of Hyrise.
It can be necessary to set the path to this version of boost manually:
```sh
export BOOST_ROOT=path/to/your/boost/installation
```

For both versions, adjust the HINTS in /cmake/FindXXXX.cmake to link to your libraries and include paths.

Now, the binaries for benchmarking the SSB can be build:
```sh
cd hyrise-pmem #same for hyrise-dram
mkdir build
cd build
cmake .. -DHYRISE_WITH_MEMKIND -DCMAKE_BUILD_TYPE=Release
make hyriseBenchmarkFileBased -j
```

### Running
Running the queries is straight-forward:
```sh
# From your build folder:
./hyriseBenchmarkFileBased --query_path ../queries --table_path /path/to/your/tables
```
## Handcrafted
### Building
To run the handcrafted ssb, it is required to build it first and set the paths to the tables:

```sh
cd handcrafted
./init_dash # Dash is used as the hashmap for hashjoins
make -DSOCKET_1_PATH=<table path> -DSOCKET_2_PATH=<table path2>
```
### Running
To run the performed benchmarks:
```sh
cd scripts/ssb

./run_ssb.sh #paths to binaries have to be adjusted to point to the binaries generated above
```

