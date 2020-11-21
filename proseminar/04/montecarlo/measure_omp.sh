#!/bin/bash

for problem_size in {100,1000,10000,100000,1000000,10000000,100000000}
do
    for num_threads in {1,2,4,8}
    do
        printf "%d threads on problem size %d\n" $num_threads $problem_size
        ./pi_omp $problem_size $num_threads
    done
done