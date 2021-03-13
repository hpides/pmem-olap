#!/bin/bash
MIN_THREADS=36
MAX_THREADS=36

AS=64

for (( AS=64; AS<=8192; AS*=2 ))
do
	for(( TC=1; TC<=36; TC+=1 ))
	do
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --dram --random --threads $TC  --ad 64 --package "/dev/dax1.1" 
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --dram --random --threads $TC --ad 128 --package "/dev/dax1.1" 
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --dram --random --threads $TC --ad 256 --package "/dev/dax1.1" 
	numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --random --threads $TC  --ad 64 --align --package "/dev/dax1.1" 
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --random --threads $TC --ad 128 --align --package "/dev/dax1.1" 
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --random --threads $TC --ad 256 --align --package "/dev/dax1.1" 
	done
done