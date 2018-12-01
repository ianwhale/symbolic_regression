#pragma once

#include <string>
#include <random>
#include <vector>
#include <memory>

class Logger;
class Population;
class Function;

struct OutgoingPayload {
    uint_fast32_t seed; // Random seed to generate samples with.
    int payload_length; // Length of incoming string.
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
    void generate_samples(std::vector<float> & samples, std::vector<float> & ground_truth,
        std::shared_ptr<Function> func, std::uniform_real_distribution<float> & domain,
        std::mt19937 & engine);
private:
    void evolve_hybrid(const int & rank, const int & size);
    void evolve_openmp();
    void evaluate_population(std::shared_ptr<Population> group,
        const std::vector<float> & samples, const std::vector<float> & ground_truth);

    float mutation_rate, crossover_rate;
    int seed; // Global seed, only used to generate random samples for master.
    int population_size, generations;
    int function;
    std::mt19937 root_engine;
    Logger * logger;
};
