# Benchmarking Source Code

This directory contains the source code for the performed SSB benchmarks.

## Building

```sh
./init_dash # Dash is used as the hashmap for hashjoins
make -DSOCKET_1_PATH=<YOUR_PATH_TO_TABLES_ON_PACKAGE_1/handcrafted> -DSOCKET_2_PATH=<YOUR_PATH_TO_TABLES_ON_PACKAGE_2/handcrafted>
```

This will give you three `ssb_pmem`, `ssb_dram` and `ssb_ssd` binaries which then can be used in the benchmarking scripts.

## Usage

Please see the `scripts/ssb` directory for example usage.

