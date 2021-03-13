#!/bin/bash
MIN_THREADS=36
MAX_THREADS=36

AS=64

for (( AS=64; AS<=8192; AS*=2 ))
do
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --threads 36 --ad $AS --package "/dev/dax1.0"
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --threads 36 --ad $AS --package "/dev/dax1.0"
    numactl --cpunodebind=0,1 --membind=0,1 ../nvm_db_benchmark --read nt --threads 36 --ad $AS --package "/dev/dax0.0"
    numactl --cpunodebind=0,1 --membind=0,1 ../nvm_db_benchmark --read nt --threads 36 --ad $AS --package "/dev/dax0.0"
done

#WARNING: ATM the package places on nvrams2 has to be calles FIRST
#The paths of the packages can vary, since every user has different access rights
#regular nt read
#../nvm_db_benchmark --read nt --threads $threads --ad 4096 --package "/mnt/nvrams2/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/150000000-Sales-Records-Test.csv"
#random nt read
#../nvm_db_benchmark --write nt --random --threads $threads --ad 4096 --package "/mnt/nvrams2/nvm-db-benchmark-tmp/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark-tmp/150000000-Sales-Records.csv"
#random store
#../nvm_db_benchmark --write clwb --random --threads $threads --ad 4096 --package "/mnt/nvrams2/nvm-db-benchmark-tmp/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark-tmp/150000000-Sales-Records.csv"
#random clwb
#../nvm_db_benchmark --write store --random --threads $threads --ad 4096 --package "/mnt/nvrams2/nvm-db-benchmark-tmp/150000000-Sales-Records.csv" --package "/mnt/nvram-gp/nvm-db-benchmark-tmp/150000000-Sales-Records.csv"