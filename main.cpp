#include <string>
#include <iostream>
#include <getopt.h>
#include "gp/driver.h"
#include "gp/function.h"

const char MUTATION_RATE = 'm';
const char CROSSOVER_RATE = 'c';
const char SEED = 's';
const char FUNCTION = 'f';
const char POPULATION_SIZE = 'p';
const char GENERATIONS = 'g';

using namespace std;

int main(int argc, char ** argv) {
    float mutation_rate = -1.0;
    float crossover_rate = -1.0;
    int seed = -1;
    int c;
    int population_size = -1;
    int generations = -1;
    FunctionFactory::FunctionType function = FunctionFactory::FunctionType::INVALID;

    //
    // Get command line arguments.
    //
    while((c = getopt(argc, argv, "m:c:s:f:p:g:")) != -1) {
        switch(c) {
            case MUTATION_RATE:
                mutation_rate = stof(optarg);

                if (mutation_rate < 0 || mutation_rate > 1) {
                    cerr << "Invalid mutation rate: " << mutation_rate << endl;
                    return 1;
                }
                break;
            case CROSSOVER_RATE:
                crossover_rate = stof(optarg);

                if (crossover_rate < 0 || crossover_rate > 1) {
                    cerr << "Invalid crossover rate: " << crossover_rate << endl;
                    return 1;
                }
                break;
            case POPULATION_SIZE:
                population_size = stoi(optarg);

                if (population_size < 2) {
                    cerr << "Invalid population size: " << population_size << endl;
                    return 1;
                }
                break;
            case GENERATIONS:
                generations = stoi(optarg);

                if (generations < 1) {
                    cerr << "Invalid number of generations: " << generations << endl;
                    return 1;
                }
                break;
            case SEED:
                seed = stoi(optarg);
                break;
            case FUNCTION:
                function = (FunctionFactory::FunctionType)stoi(optarg);
                break;

            default:
                cerr << "Invalid usage commnand line arguments, exiting..." << endl;
                return 1;
        }
    }

    if (mutation_rate == -1.0 ||
        crossover_rate == -1.0 ||
        seed == -1 ||
        function == FunctionFactory::FunctionType::INVALID ||
        population_size == -1 ||
        generations == -1) {
            cerr << "***********************************" << endl;
            cerr << "*Invalid configuration, exiting...*" << endl;
            cerr << "***********************************" << endl;
            return 1;
    }

    // Construct the driver and start computation.
    Driver driver = Driver(mutation_rate, crossover_rate,
                    seed, function, population_size, generations);
    driver.evolve();
    return 0;
}
