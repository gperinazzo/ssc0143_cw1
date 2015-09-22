#!/bin/bash
# ./run.sh <input_file>


mkdir -p output

# Run 10 times the sequential algoritm
for i in `seq 1 10`
do
    echo Executing sequential ${1}.txt iteration ${i} of 10
    (/usr/bin/time -f%e bin/Trab1Conc_seq < data/${1}.txt) 2>> output/seq_${1}.out > output/seq_${1}_result.out
done

for i in `seq 2 8`
do
    for j in `seq 1 10`
    do
        echo Executing parallel ${1}.txt with ${i} threads iteration ${j} of 10
        (/usr/bin/time -f%e bin/Trab1Conc_par ${i} < data/${1}.txt) 2>> output/par_${1}_${i}.out > output/par_${1}_${i}_result.out
    done
    
done
