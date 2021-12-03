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
The reproducibilty contains two parts: 1. The microbenchmarks and 2. The Star Schema benchmarks.

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

Star Schema Benchmarks(SSB):
We provide three implementations of the SSB, which can be found in ${REPOSITORY_ROOT}/ssb.
1. (handcrafted) A handcrafted version using fixed tuple sizes and Dash
2. (hyrise-dram) Using the HYRISE column-store database on DRAM
3. (hyrise-pmem) Using the HYRISE column-store database storing all tables and intermediates in PMEM

0. Get the required tables
    Both the handcrafted version as well as the HYRISE version require pre-generated tables to work.
    While you can create them yourself, we DO NOT recommend doing this yourself and instead use the tables we provide via:
    a) Get the files via sftp: sftp <link provided in email>, then: get pmem-olap-reproducability-tables.tar.gz
    b) Unpack the tables: tar -xf pmem-olap-reproducability-tables.tar.gz
    c) Copy the tables to both of your PMEM packages 

    If you want to generate the tables yourself (NOT RECOMMENDED):
    a*) switch into ${REPOSITORY_ROOT}/ssb/ssb-dbgen
    ba*) For HYRISE: Run make; ./dbgen -s 100
    bb*) For handcrafted: Run make; ./dbgen -s 50
    ca*) For HYRISE: Copy the tables to both of your PMEM packages
    cb*) For handcrafted: Align the format of the tables to 128byte, with field widths according to ${REPOSITORY_ROOT}/ssb/handcrafted/ssb.cpp and copy the resulting tables to both of your PMEM packages.

1. Handcrafted:
    a)  Build the binaries according to ${REPOSITORY_ROOT}/ssb/handcrafted/README.md
    b)  Change the paths in  ${REPOSITORY_ROOT}/scripts/reproducability/ssb/run_ssb.sh to match your binaries,
        as well as your devdax paths.
    c) If necessary, rm -r ${REPOSITORY_ROOT}/bm_results  ${REPOSITORY_ROOT}/bm_results_plots    
    d) Run 
        ${REPOSITORY_ROOT}/scripts/reproducability/ssb/run_ssb.sh 
        to produce .csvs containing the results in ${REPOSITORY_ROOT}/bm_results

2/3. Hyrise DRAM/PMEM:
    The instructions to run the SSB on PMEM and DRAM are very similar:
    a) Build the binaries according to ${REPOSITORY_ROOT}/ssb/hyrise-*/README.md
    b) Second, from your build folder (assuming ${REPOSITORY_ROOT}/ssb/hyrise-*/build), run
        ./hyriseBenchmarkFileBased --query_path ../queries --table_path /path/to/your/tables
        to get the results from each query printed to stdout.

