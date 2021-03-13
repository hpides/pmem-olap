#!/bin/bash

BM_BINARY="/hpi/fs00/home/lars.bollmeier/nvm-db-benchmark/src/nvm_db_benchmark"
BM_WRITE_FENCE_BINARY="/hpi/fs00/home/lars.bollmeier/nvm-db-benchmark/src/nvm_db_benchmark_fenced"
RESULT_FOLDER="bm_results_dram"
NUMA_SETTING="numactl --cpunodebind=2,3 --membind=2,3"
DEV_DAX1_PATH="/dev/dax1.1"


if [ -d $RESULT_FOLDER ]; then
	echo "Resultfolder ${RESULT_FOLDER} already exists. Remove it, before starting to benchmark."
	exit 0
fi

mkdir $RESULT_FOLDER

#Sequential read benchmarks
#	Logstyle
declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=33554432

 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_read_log_${threads}.csv"
         done
done

#	disjoint
declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=33554432

 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --access_pattern disjoint --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_read_disjoint_${threads}.csv"
         done
done

#sequential write
#	log style
#		unfenced
declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=33554432

 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_write_log_${threads}.csv"
         done
 done
#		fenced
 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_WRITE_FENCE_BINARY \
                 --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_write_log_fenced_${threads}.csv"
         done
 done

#	disjoint style
#		unfenced
declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=33554432

 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_write_disjoint_${threads}.csv"
         done
 done
#		fenced
 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_WRITE_FENCE_BINARY \
                 --read nt --access_pattern log --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/sequential_write_disjoint_fenced_${threads}.csv"
         done
 done

#random read
#unaligned
declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=33554432
 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --random --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_read_unaligned_${threads}.csv"
         done
 done

#unaligned
declare -a THREADS=("1" "4" "8" "16" "18" "24" "32" "36")
MIN_AD=64
MAX_AD=33554432
 for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --read nt --random --align --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_read_aligned_${threads}.csv"
         done
 done

#random write
	#unaligned
		#unfenced
declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=33554432
for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --write nt --random --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_write_unaligned_${threads}.csv"
         done
done
		#fenced
for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_WRITE_FENCE_BINARY \
                 --write nt --random --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_write_unaligned_fenced_${threads}.csv"
         done
done

	#aligned
		#unfenced
declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=33554432
for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_BINARY \
                 --write nt --random --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_write_aligned_${threads}.csv"
         done
done
		#fenced
declare -a THREADS=("1" "2" "4" "6" "8" "18" "24" "36")
MIN_AD=64
MAX_AD=33554432
for threads in "${THREADS[@]}"
 do
         for (( ad=$MIN_AD; ad<=$MAX_AD; ad *= 2 ))
         do
                $NUMA_SETTING $BM_WRITE_FENCE_BINARY \
                 --write nt --random --measure_group 0 --threads $threads --ad $ad --package $DEV_DAX1_PATH | tee -a "${RESULT_FOLDER}/random_write_aligned_fenced_${threads}.csv"
         done
done