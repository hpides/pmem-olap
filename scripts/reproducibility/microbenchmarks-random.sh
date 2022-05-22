#!/bin/bash

REPOSITORY_ROOT="/YOUR_PATH_TO_PMEM_OLAP_REPO"

BM_BINARY="${REPOSITORY_ROOT}/microbenchmarks/build/nvm_db_benchmark"
BM_BINARY_FENCED="${REPOSITORY_ROOT}/microbenchmarks/build/nvm_db_benchmark_fenced"

PLOT_SCRIPTS="${REPOSITORY_ROOT}/plot_scripts"

RESULT_FOLDER="bm_results"
RESULT_FOLDER_PMEM="${RESULT_FOLDER}/pmem"
RESULT_FOLDER_DRAM="${RESULT_FOLDER}/dram"
PLOTS_FOLDER="bm_results_plots"

NUMA_SETTING0="numactl --cpunodebind=0,1 --membind=0,1"
NUMA_SETTING1="numactl --cpunodebind=2,3 --membind=2,3"

DEV_DAX0_PATH="/dev/dax0.0"
DEV_DAX1_PATH="/dev/dax1.1"

MAPPING_FILE0="${REPOSITORY_ROOT}/microbenchmarks/mapping_files/socket_1_split"
MAPPING_FILE1="${REPOSITORY_ROOT}/microbenchmarks/mapping_files/socket_2_split"

if [ -d $RESULT_FOLDER ]; then
	echo "Resultfolder ${RESULT_FOLDER} already exists. Remove it, before starting to benchmark."
	exit 0
fi

mkdir $RESULT_FOLDER
mkdir $RESULT_FOLDER_PMEM
mkdir $RESULT_FOLDER_DRAM

echo "Attention: Before Running this script, make sure to set the DEVDAXSIZE in microbenchmarks/include to 2GB and build again to match the results from the paper!" 

# Figure 12
# a)

echo "Running benchmarks for Figure 12a)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=8192
for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/random_read_${threads}.csv"
        done
done

# b)

echo "Running benchmarks for Figure 12b)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=8192
for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH --dram > /dev/null
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/random_read_${threads}.csv"
        done
done

# Figure 13
# a)

echo "Running benchmarks for Figure 13a)..."

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=8192
for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/random_write_${threads}.csv"
        done
done

# b)

echo "Running benchmarks for Figure 13b)..."

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=8192
for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH --dram > /dev/null
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/random_write_${threads}.csv"
        done
done
