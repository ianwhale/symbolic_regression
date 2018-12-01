#!/bin/bash
export OMP_NUM_THREADS=2,1
mpirun -n 2 ./run.out -m 0.01 -c 0.75 -s 0 -f 4 -p 101 -g 1 -o ./simple_run
