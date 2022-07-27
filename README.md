# Maximizing Persistent Memory Bandwidth Utilization for OLAP Workloads

This repository contains the source code for our [ACM SIGMOD '21 paper](https://hpi.de/fileadmin/user_upload/fachgebiete/rabl/publications/2021/pmem_olap_sigmod_21.pdf).

If you use this in your work, please cite us.
```
@inproceedings{daase_pmem_olap_21,
    address = {Virtual Event, China},
    title = {Maximizing Persistent Memory Bandwidth Utilization for OLAP Workloads},
    booktitle = {Proceedings of the 2021 International Conference on Management of Data (SIGMOD '21), June 20--25, 2021, Virtual Event, China},
    publisher = {ACM},
    author = {Björn Daase and Lars Jonas Bollmeier and Lawrence Benson and Tilmann Rabl},
    year = {2021}
}
```


## Table of Content

Directory | Description
----------|------------
[microbenchmarks/](https://github.com/hpides/pmem-olap/tree/master/microbenchmarks) | Source code of the microbenchmarks
[plot_scripts/](https://github.com/hpides/pmem-olap/tree/master/plot_scripts) | Scripts to generate the graphics
[scripts/](https://github.com/hpides/pmem-olap/tree/master/scripts) | Scripts to perform the benchmarks
[ssb/](https://github.com/hpides/pmem-olap/tree/master/ssb) | Source code of the Star Schema Benchmark implementation

You probably want to step through theses directories in the following order

1. Start by building the binaries in `microbenchmarks`.
2. Use (or get inspired by) the benchmarking scripts in `scripts` to generate data.
3. Visualize this data with the plotting scripts found in `plot_scripts`.
4. You can explore the code/setup in `ssb` if you are interested in the SSB benchmarks. The setup is rather complex and we have removed all of Hyrise's dependencies, so this code will not be able to build out of the box. The code is only provided for reference.


## License
```
# SPDX-License-Identifier: GPL-2.0
```
