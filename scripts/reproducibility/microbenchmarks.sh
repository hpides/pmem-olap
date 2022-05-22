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

if [ -d $PLOTS_FOLDER ]; then
	echo "Plotfolder ${PLOTS_FOLDER} already exists. Remove it, before starting to benchmark."
	exit 0
fi

mkdir $PLOTS_FOLDER


echo "Started generation of benchamrking results. THIS WILL TAKE A LONG TIME!"

# Figure 3
# a)

echo "Running benchmarks for Figure 3a)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=65536

for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_log_${threads}.csv"
        done
done

# b)

echo "Running benchmarks for Figure 3b)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=65536

for threads in "${THREADS[@]}"
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY \
                --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_disjoint_${threads}.csv"
        done
done

# Figure 4

echo "Running benchmarks for Figure 4..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")

for threads in "${THREADS[@]}"
do

    $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_pinning_none.csv"
    
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_pinning_numa.csv"

    $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  --mapping_file $MAPPING_FILE1 > /dev/null
    $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  --mapping_file $MAPPING_FILE1 | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_pinning_cores.csv"
done

# Figure 5

echo "Running benchmarks for Figure 5..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")

for threads in "${THREADS[@]}"
do
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_near.csv"
    
    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_far.csv"

    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_far_second.csv"
done


# Figure 6
# a)

echo "Running benchmarks for Figure 6a)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")

for threads in "${THREADS[@]}"
do
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_1_near.csv"

    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_1_far.csv"

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_2_near.csv"

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_2_far.csv"   

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_read_1_near_1_far.csv"    
done


# b)

echo "Running benchmarks for Figure 6b)..."

declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")

for threads in "${THREADS[@]}"
do
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $NUMA_SETTING1 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_read_1_near.csv"

    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $NUMA_SETTING0 $BM_BINARY \
        --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_read_1_far.csv"

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_read_2_near.csv"

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_read_2_far.csv"   

    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $BM_BINARY \
        --read nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --read nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_read_1_near_1_far.csv"    
done

# Figure 7/8
# a)

echo "Running benchmarks for Figure 7a)..."

MIN_AD=64
MAX_AD=65536

for threads in {1..36}
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_log_fenced_${threads}.csv" "${RESULT_FOLDER_PMEM}/sequential_write_log_fenced.csv"
        done
done

# b)

echo "Running benchmarks for Figure 7b)..."

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=65536

for threads in {1..36}
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
        do
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH > /dev/null
            $NUMA_SETTING1 $BM_BINARY_FENCED \
                --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_disjoint_fenced_${threads}.csv" "${RESULT_FOLDER_PMEM}/sequential_write_disjoint_fenced.csv"
        done
done

# Figure 9

echo "Running benchmarks for Figure 9..."

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")

for threads in "${THREADS[@]}"
do

    $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_pinning_none.csv"
    
    $NUMA_SETTING1 $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $NUMA_SETTING1 $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_pinning_numa.csv"

    $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  --mapping_file $MAPPING_FILE1 > /dev/null
    $BM_BINARY \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  --mapping_file $MAPPING_FILE1 | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_pinning_cores.csv"
done

# Figure 10

echo "Running benchmarks for Figure 10..."

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")

for threads in "${THREADS[@]}"
do
    $NUMA_SETTING1 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $NUMA_SETTING1 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_1_near.csv"

    $NUMA_SETTING0 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  > /dev/null
    $NUMA_SETTING0 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_1_far.csv"

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH  \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH  \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_2_near.csv"

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH  \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH  > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_2_far.csv"   

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_write_1_near_1_far.csv"    
done

declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")

for threads in "${THREADS[@]}"
do
    $NUMA_SETTING1 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $NUMA_SETTING1 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_write_1_near.csv"

    $NUMA_SETTING0 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $NUMA_SETTING0 $BM_BINARY_FENCED \
        --write nt --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_write_1_far.csv"

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_write_2_near.csv"

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX0_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_write_2_far.csv"   

    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram > /dev/null
    $BM_BINARY_FENCED \
        --write nt --numa "0,1" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram \
        --write nt --numa "2,3" --access_pattern disjoint --measure_group 0 --threads $threads --ad 4096 --package $DEV_DAX1_PATH --dram | tee -a "${RESULT_FOLDER_DRAM}/sequential_write_1_near_1_far.csv"    
done



# Figure 11

echo "Running benchmarks for Figure 11..."

# Read
declare -a WRITE_THREADS=("1" "4" "6")
declare -a READ_THREADS=("1" "8" "18" "30")
MIN_AD=4096 
MAX_AD=4096

for write_threads in "${WRITE_THREADS[@]}"
do
	for read_threads in "${READ_THREADS[@]}"
	do
		
		{ while true; do
		 	$NUMA_SETTING1 $BM_BINARY \
                	--write nt --access_pattern disjoint --measure_group 0 --threads $write_threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null;
		done; } &
		
		FIRST=$!
		{
			$NUMA_SETTING1 $BM_BINARY \
                	--read nt --access_pattern disjoint --measure_group 0 --threads $read_threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_mixed_write.csv"
		} &
		
		LAST=$!
		wait $LAST
		kill $FIRST > /dev/null
		kill $LAST > /dev/null
		sleep 2
	done
done

# Write
declare -a WRITE_THREADS=("1" "4" "6")
declare -a READ_THREADS=("1" "8" "18" "30")
MIN_AD=4096 
MAX_AD=4096

for write_threads in "${WRITE_THREADS[@]}"
do
	for read_threads in "${READ_THREADS[@]}"
	do
		
		{ while true; do
		 	$NUMA_SETTING1 $BM_BINARY \
                	--read nt --access_pattern disjoint --measure_group 0 --threads $write_threads --ad 4096 --package $DEV_DAX1_PATH > /dev/null;
		done; } &
		
		FIRST=$!
		{
			$NUMA_SETTING1 $BM_BINARY \
                	--write nt --access_pattern disjoint --measure_group 0 --threads $read_threads --ad 4096 --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER_PMEM}/sequential_mixed_read.csv"
		} &
		
		LAST=$!
		wait $LAST
		kill $FIRST > /dev/null
		kill $LAST > /dev/null
		sleep 2
	done
done



echo "Finished generation of all benchmarking results!. Now, please copy all data to your local machine, uncomment the second part below this line of the script and comment out the part above this line and rerun the script."

# echo "Generating plots..."

# REPOSITORY_ROOT="/YOUR_PATH_TO_PMEM_OLAP_REPO"

# PLOT_SCRIPTS="${REPOSITORY_ROOT}/plot_scripts"
# RESULT_FOLDER="/home/bjoern/bm_results"
# RESULT_FOLDER_PMEM="${RESULT_FOLDER}/pmem"
# RESULT_FOLDER_DRAM="${RESULT_FOLDER}/dram"
# PLOTS_FOLDER="/home/bjoern/bm_results_plots"

# python3 "${PLOT_SCRIPTS}/optimal_access_size.py" $RESULT_FOLDER_PMEM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/pinning_variants.py" $RESULT_FOLDER_PMEM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/warm_up_behavior.py" $RESULT_FOLDER_PMEM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/multiple_sockets.py" $RESULT_FOLDER_PMEM $RESULT_FOLDER_DRAM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/mixed_performance.py" $RESULT_FOLDER_PMEM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/random_access.py" $RESULT_FOLDER_PMEM $RESULT_FOLDER_DRAM $PLOTS_FOLDER
# python3 "${PLOT_SCRIPTS}/heatmap.py" $RESULT_FOLDER_PMEM $PLOTS_FOLDER
