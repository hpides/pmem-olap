#!/bin/bash
MIN_THREADS=1
MAX_THREADS=72
MIN_AD=64
MAX_AD=67108864

for (( threads=$MIN_THREADS; threads<=$MAX_THREADS; ++threads ))
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad*=2 ))
        do
                ./nvm_db_benchmark --read nt --random --threads $threads --ad $ad --package "/mnt/nvrams2/nvm-db-benchmark/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark/150000000-Sales-Records.csv" | tee -a read_nt_random_test.csv
        done
done

for (( threads=$MIN_THREADS; threads<=$MAX_THREADS; ++threads ))
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad*=2 ))
        do
                ./nvm_db_benchmark --read nt --threads $threads --ad $ad --package "/mnt/nvrams2/nvm-db-benchmark/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark/150000000-Sales-Records.csv" | tee -a read_nt_test.csv
        done
done

MIN_THREADS=1
MAX_THREADS=72
MIN_AD=128
MAX_AD=8192

for (( threads=$MIN_THREADS; threads<=$MAX_THREADS; ++threads ))
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad+=128 ))
        do
                ./nvm_db_benchmark --read nt --random --threads $threads --ad $ad --package "/mnt/nvrams2/nvm-db-benchmark/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark/150000000-Sales-Records.csv" | tee -a read_nt_random_fine_test.csv
        done
done

for (( threads=$MIN_THREADS; threads<=$MAX_THREADS; ++threads ))
do
        for (( ad=$MIN_AD; ad<=$MAX_AD; ad+=128 ))
        do
                ./nvm_db_benchmark --read nt --threads $threads --ad $ad --package "/mnt/nvrams2/nvm-db-benchmark/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark/150000000-Sales-Records.csv" | tee -a read_nt_fine_test.csv
        done
done
