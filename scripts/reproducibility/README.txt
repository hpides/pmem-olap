Maximizing Persistent Memory Bandwidth Utilization for OLAP Workloads (Submission #433)
--------------------------------------------------------------------------------------

This document complements the publication "Maximizing Persistent Memory Bandwidth Utilization for OLAP Workloads" with information on how to reproduce our experimental results.

A) Source code info
URL to the code: https://github.com/hpides/pmem-olap
Programming language: C++/C/Assembly



B)
The data for the microbenchmarks will be generated on the fly.
The tables for the Hyrise benchamrks can be found here: 



C) Hardware Info
CPU: 2 x Intel® Xeon® Gold 5220S
Caches: L1d: 1.1 MiB, L1i: 1.1 MiB, L2: 36 MiB, L3: 49.5 MiB
DRAM: 6 GB x 6 channels x 2 sockets (DDR 4, clocked down to PMEM speed)
PMEM: 128 GB x 6 DIMMs x 2 sockets
SSD: Intel® DC P4610 Serie

If you need access to this kind of hardware (especially PMEM), please drop us an e-mail.



D) Experimentation Info
The reproducibilty contains two parts: 1. The microbenchmarks and 2. The Hyrise benchmarks.

Microbenchmarks:
---PMEM Server---
1. First, on the PMEM server, build the benchmarking binaries by following the instructions in ${REPOSITORY_ROOT}/microbenchmarks/README.md.
2. Please have a look at the microbenchmarks.sh file and adjust line 3 according to your system setup.
Then, execute the script on the server. This will take ~72 hours.
3. Next, copy the microbenchmark.sh script and the result folders, bm_results and bm_results_plots, onto your local machine (this is necessary because we do not have permissions to install the
required python packages on our server).
---Local Plot Generation Machine---
4. On your local copy of the repository, uncomment the second part below line 455 of the microbenchmarks.sh file and comment out the part above line 455 and rerun the script.
Also, adjust line 459 according to your system setup.
5. Now you can find the final images in bm_results_plots.

Hyrise:
