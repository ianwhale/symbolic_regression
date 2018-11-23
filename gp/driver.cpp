#include <random>
#include <memory>
#include <vector>
#include "mpi.h"
#include "omp.h"
#include "logger.h"
#include "function.h"
#include "evolution.h"
#include "evaluation.h"
#include "population.h"
#include "individual.h"
#include "driver.h"

using std::mt19937;
using std::uniform_real_distribution;
using std::shared_ptr;


/**
 * Constructor.
 * @param _mutation_rate   float, [0, 1]
 * @param _crossover_rate  float, [0, 1]
 * @param _seed            int, random seed.
 * @param _population_size int, > 2
 * @param _generations     int, > 1
 */
Driver::Driver(float _mutation_rate, float _crossover_rate, int _seed, int _function,
        int _population_size, int _generations, std::string _output_dir) :
             mutation_rate(_mutation_rate), crossover_rate(_crossover_rate),
             function(_function), root_engine(_seed),
             population_size(_population_size),
             generations(_generations) {
    this->logger = new Logger(_output_dir);
}


/**
 * Evolve with OpenMP only.
 */
void Driver::evolve_openmp() {
    auto population = make_shared<Population>(this->population_size);
    population->initialize(this->root_engine, 2, 6);

    auto func = FunctionFactory::make_function((FunctionFactory::FunctionType)this->function);
    vector<float> samples(Evaluation::NUM_SAMPLES, 0);
    vector<float> ground_truth(Evaluation::NUM_SAMPLES, 0);
    auto dom = func->domain();
    uniform_real_distribution<float> domain(dom.first, dom.second);
    double start_time;

    // Start thread team to avoid creation and destruction at every generation.
    #pragma omp parallel
    #pragma omp single
    {
        for (int current_generation = 0; current_generation <= this->generations;
            current_generation++) {
            // Evaluate each individual in the population.
            // Generate random samples for evaluation.
            for (int s = 0; s < Evaluation::NUM_SAMPLES; s++) {
                samples[s] = domain(this->root_engine);
                ground_truth[s] = func->call(samples[s]);
            }

            start_time = omp_get_wtime();
            for (int i = 0; i < this->population_size; i++) {
                #pragma omp task
                {
                    // Set each individual's fitness.
                    (*population)[i]->set_fitness(
                        Evaluation::assign_rmse(
                            (*population)[i]->get_tree()->get_rpn_string(),
                            samples, ground_truth)
                    );
                }
            }
            // Wait for everyone to be evaluated.
            #pragma taskwait

            // Log results of the evaluation.
            this->logger->log(population, current_generation,
                omp_get_wtime() - start_time);

            // Do evolution step: selection, crossover, and mutation.
            population->update(this->root_engine);
        }
    }
}

void Driver::evolve_hybrid(const int & rank) {}

/**
 * Main evolution function.
 * @param argc int, argument count.
 * @param argv char **, argument array.
 */
void Driver::evolve(int argc, char ** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // More than one rank, enter hybrid mode.
    if (size > 1) {
        this->evolve_hybrid(rank);
    }
    // One rank, use OpenMP only.
    else {
        MPI_Finalize(); // Don't need MPI anymore.
        this->evolve_openmp();
        return; // Avoid calling MPI_Finalize twice.
    }

    MPI_Finalize();
}
