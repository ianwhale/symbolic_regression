#!/bin/bash

if [ ! -f ./catch_main.o ]; then
    g++ --std=c++11 catch_main.cpp -o catch_main.o
fi

for dir in $(find . -maxdepth 1 -mindepth 1 -type d);
do
    for test in $dir/*.cpp;
    do
        g++ --std=c++11 catch_main.o $test -fopenmp
        ./a.out
    done
done
