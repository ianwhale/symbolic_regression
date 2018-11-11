# CSE822 Symbolic Regression Project

A simple symbolic regression project using genetic programming, MPI, and OpenMP.

## Compiling

TBA

## Command line

Compiled program takes a few arguments to be able to run.

```
-m <float> mutation rate, in range [0, 1]
-c <float> crossover rate, in range [0, 1]
-s <int> seed, random seed
-f <int> function, from FunctionFactory::FunctionType enum
-p <int> population size, > 0
-g <int> generations, > 0
```

Each must be provided, no default values are hard coded.
