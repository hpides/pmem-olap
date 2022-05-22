# Star Schema Benchmark(SSB) On HYRISE

For the requirements, please run `install_dependencies.sh` and/or consider the DEPENDENCIES.md
In addition, you will need:
```sh
libpmem
libpmem2
libmemkind
boost_1_73_0
```
The older version of boost is needed as in newer versions some functions have been moved, which breaks this outdated version of Hyrise.
It can be necessary to set the path to this version of boost manually:

```sh
export BOOST_ROOT=path/to/your/boost/installation
export PostgreSQL_ROOT=/path/to/your/postgresql/installation
```

For both versions, adjust the HINTS in /cmake/FindXXXX.cmake to link to your libraries and include paths.

Now, the binaries for benchmarking the SSB can be build:
```sh
mkdir build
cd build
cmake .. -DHYRISE_WITH_MEMKIND=TRUE -DCMAKE_BUILD_TYPE=Release
make hyriseBenchmarkFileBased -j
```

Before running, you still have to adjuse the nvm.json file to point to an empty directory placed on PMEM.
