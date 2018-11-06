#pragma once

#include "function.h"

class Driver {
public:
    /**
     * Constructor.
     * @param _mutation_rate   float, [0, 1]
     * @param _crossover_rate  float, [0, 1]
     * @param _seed            int, random seed.
     * @param _population_size int, > 2
     * @param _generations     int, > 1
     */
    Driver(float _mutation_rate, float _crossover_rate, int _seed,
           FunctionFactory::FunctionType _function,
           int _population_size, int _generations) :
                mutation_rate(_mutation_rate), crossover_rate(_crossover_rate),
                function(_function), seed(_seed),
                population_size(_population_size),
                generations(_generations) {}

    /**
     * Do the evolution loop.
     */
    void evolve() {
        // Do evolution.
    }

private:
    float mutation_rate, crossover_rate;
    int seed, population_size, generations;
    FunctionFactory::FunctionType function;
    
};
