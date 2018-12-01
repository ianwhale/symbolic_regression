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
const char OUTPUT = 'o';

using namespace std;

int main(int argc, char ** argv) {
    float mutation_rate = -1.0;
    float crossover_rate = -1.0;
    int seed = -1;
    int c;
    int population_size = -1;
    int generations = -1;
    int function = -1;
    string output_dir = "";

    //
    // Get command line arguments.
    // Arguments are:
    //  -m <float> mutation rate, in range [0, 1]
    //  -c <float> crossover rate, in range [0, 1]
    //  -s <int> seed, random seed
    //  -f <int> function, from FunctionFactory::FunctionType enum
    //  -p <int> population size, > 0
    //  -g <int> generations, > 0
    //
    while((c = getopt(argc, argv, "m:c:s:f:p:g:o:")) != -1) {
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

                if (generations < 0) {
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

            case OUTPUT:
                output_dir = optarg;
                break;

            default:
                cerr << "Invalid usage commnand line arguments, exiting..." << endl;
                return 1;
        }
    }

    // Check setup validity.
    if (mutation_rate == -1.0 ||
        crossover_rate == -1.0 ||
        seed == -1 ||
        function == FunctionFactory::FunctionType::INVALID ||
        population_size == -1 ||
        generations == -1 ||
        output_dir == "") {
            cerr << "***********************************" << endl;
            cerr << "*Invalid configuration, exiting...*" << endl;
            cerr << "***********************************" << endl;
            return 1;
    }

    // Construct the driver and start computation.
    Driver driver(mutation_rate, crossover_rate, seed, function,
                           population_size, generations, output_dir);

    driver.evolve(argc, argv);
    return 0;
}
