#!/bin/bash
MIN_THREADS=36
MAX_THREADS=36

AS=64

for (( AS=64; AS<=8192; AS*=2 ))
do
	for(( TC=1; TC<=36; TC+=1 ))
	do
    # numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --threads $TC --ad $AS --package "/dev/dax1.1" --access_pattern log
    # numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --threads $TC --ad $AS --package "/dev/dax1.1" --access_pattern disjoint
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --dram --threads $TC --ad $AS --package "/dev/dax1.1" --access_pattern log
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --read nt --dram --threads $TC --ad $AS --package "/dev/dax1.1" --access_pattern disjoint
	done
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