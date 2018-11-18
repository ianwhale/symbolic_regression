#!/bin/bash

function compile_and_run_tests_in_dir {
    for test in $1/*.cpp;
    do
        g++ --std=c++11 catch_main.o $test -fopenmp
        ./a.out
    done
}

if [ ! -f ./catch_main.o ]; then
    g++ --std=c++11 catch_main.cpp -o catch_main.o
fi

if [ $# -gt 0 ]; then
    # Do only tests in directories provided.
    while [ "$1" != "" ]; do
        compile_and_run_tests_in_dir $1
        shift
    done
else
    # Do all tests.
    for dir in $(find . -maxdepth 1 -mindepth 1 -type d);
    do
        compile_and_run_tests_in_dir $dir
    done
fi

rm a.out
