# Run SSB
This is a instruction to reproduce the numbers presented in chapter 6, especially figures 14 and 15.

---
## Hyrise Build

To be able to run this, several dependencies for this version of hyrise have to be resolved:
# download both the hyrise version with dram as well as the pmem version
```
git clone
git clone
```

in one of both repositories run:
```
git submodule init
git submodule update
./install_dependencies.sh
 ```
In addition, install

```
libpmem
libpmem2
libmemkind
```
as well as 
```
boost_1_73_0
```
If boost is not at the default install location, run 
```
export BOOST_ROOT=YourBoostPath
```
before proceeding.
Now, the package files in cmake/Find* must be adjusted according to your installation paths, so Hyrise can use the libraries listed above.

Building Hyrise itself is straighforward:
```
mkdir build
./install_dependencies.sh
pushd build

```
For the DRAM version:
```
cmake .. -DCMAKE_BUILD_TYPE=Release
```
For the PMEM version:
Change the files in the nvm.json file to fit to your pmem location on your system, then:
```
cmake .. -DCMAKE_BUILD_TYPE=Release -DHYRISE_WITH_MEMKIND=TRUE
```
Finally, for both versions:
```
make hyriseBenchmarkFile -j
```



Now, Table data has to be generated or downloaded.
**Warning: The generation of the tables takes a very long time. We recommend downloading the tables directly**
For this, we used ssb-dbgen
```
git clone https://github.com/electrum/ssb-dbgen.git
cd ssb-dbgen
make
```

Now, generate table data with a scaling factor of 100 for Hyrise:
```
./ssb-dbgen -s 100
cp -r *.tbl $YourHyriseFolder$/tables
```

For the handcrafted SSB, we created a copy of those tables, aligned every field to a fixed length and added NULL-byte padding at the end, so every tuple reaches a length of 128 byte.

# Handcrafted SSB build
```
cd nvm-db-benchmark
cd ssb
make
cd ../ssb_split
make

Now, from both Hyrise directories, run:

```
./build/hyriseBenchmarkFileBased

```