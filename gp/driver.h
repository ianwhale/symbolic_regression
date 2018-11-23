#pragma once

#include <string>
#include <random>

class Logger;


struct OutgoingPayload {
    int seed; // Random seed to generate random samples with.
    int payload_length; // Length of incoming string.
};

struct IncomingPayload {
    int index; // Index of individual in the population.
    float fitness; // Fitness of individual at the provided index.
};


class Driver {
public:

    const int MASTER = 0; // Rank of the master process.

    /**
     * Constructor.
     * @param _mutation_rate   float, [0, 1]
     * @param _crossover_rate  float, [0, 1]
     * @param _seed            int, random seed.
     * @param _population_size int, > 2
     * @param _generations     int, > 1
     */
    Driver(float _mutation_rate, float _crossover_rate, int _seed, int _function,
           int _population_size, int _generations, std::string _output_dir);

    void evolve(int argc, char ** argv);

private:
    void evolve_hybrid(const int & rank);
    void evolve_openmp();

    float mutation_rate, crossover_rate;
    int seed; // Global seed, only used to generate random samples for master.
    int population_size, generations;
    int function;
    std::mt19937 root_engine;
    Logger * logger;
};
