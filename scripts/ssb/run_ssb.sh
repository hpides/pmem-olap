#!/bin/bash

PMEM_BINARY="/YOUR_PATH_TO_PMEM_OLAP_REPO/ssb/ssb_pmem.out"
DRAM_BINARY="/YOUR_PATH_TO_PMEM_OLAP_REPO/ssb_dram.out"

RESULT_FOLDER="bm_results"
DEV_DAX_PATH1="/dev/dax0.0"
DEV_DAX_PATH2="/dev/dax1.1"

if [ -d $RESULT_FOLDER ]; then
	echo "Resultfolder ${RESULT_FOLDER} already exists. Remove it, before starting to benchmark."
	exit 1
fi
mkdir $RESULT_FOLDER

declare -a QFS=("1" "2" "3" "4")
declare -a QUERIES=("1" "2" "3" "4")

# All Queries
for qf in "${QFS[@]}"
do
    for q in "${QUERIES[@]}"
    do
        if [ $qf != "3" ] && [ $q == "4" ]; then
            continue
        fi

        $PMEM_BINARY $qf $q 4096 18 1 1 1 | tee -a "${RESULT_FOLDER}/ssb_pmem.csv"
        $DRAM_BINARY $qf $q 4096 18 1 1 1 | tee -a "${RESULT_FOLDER}/ssb_dram.csv"
    done
done

# Optimization Steps
PMEM_RESULTS="${RESULT_FOLDER}/ssb_pmem_opt_steps.csv"
DRAM_RESULTS="${RESULT_FOLDER}/ssb_dram_opt_steps.csv"

echo "Single Threaded"
$PMEM_BINARY 2 1 4096 1 0 0 0 | tee -a $PMEM_RESULTS
$DRAM_BINARY 2 1 4096 1 0 0 0 | tee -a $DRAM_RESULTS

echo "Multi Threaded"
$PMEM_BINARY 2 1 4096 18 0 0 0 | tee -a $PMEM_RESULTS
$DRAM_BINARY 2 1 4096 18 0 0 0 | tee -a $DRAM_RESULTS

echo "Multi-Socket"
$PMEM_BINARY 2 1 4096 18 0 1 0 | tee -a $PMEM_RESULTS
$DRAM_BINARY 2 1 4096 18 0 1 0 | tee -a $DRAM_RESULTS

echo "NUMA"
$PMEM_BINARY 2 1 4096 18 0 1 1 | tee -a $PMEM_RESULTS
$DRAM_BINARY 2 1 4096 18 0 1 1 | tee -a $DRAM_RESULTS

echo "Explicit Pinning"
$PMEM_BINARY 2 1 4096 18 1 1 1 | tee -a $PMEM_RESULTS
$DRAM_BINARY 2 1 4096 18 1 1 1 | tee -a $DRAM_RESULTS


# Multiple Workers

declare -a THREADS=("18" "9" "3" "2" "1")
for threads in "${THREADS[@]}"
do
	$PMEM_BINARY 2 1 4096 $threads 0 1 1 1 | tee -a "${RESULT_FOLDER}/ssb_pmem_workers.csv"
done
