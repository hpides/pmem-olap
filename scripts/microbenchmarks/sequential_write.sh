#!/bin/bash
MIN_THREADS=36
MAX_THREADS=36

AS=64

for (( AS=64; AS<=8192; AS*=2 ))
do
	for(( TC=1; TC<=10; TC+=1 ))
	do
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --threads $TC --ad $AS --package "/dev/dax1.0" --access_pattern log
    numactl --cpunodebind=2,3 --membind=2,3 ../nvm_db_benchmark --write nt --threads $TC --ad $AS --package "/dev/dax1.0" --access_pattern disjoint
	done
done