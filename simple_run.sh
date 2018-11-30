#!/bin/bash
export OMP_NUM_THREADS=4,1
./run.out -m 0.01 -c 0.75 -s 0 -f 4 -p 100 -g 1000 -o ./simple_run
