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

#include <iostream>
using std::cout; using std::endl;

using std::mt19937;
using std::uniform_real_distribution;
using std::shared_ptr;
using std::vector;
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
 * Evaluates a group of individuals.
 * @param group vector of pointers to individuals.
 */
void Driver::evaluate_group(vector<shared_ptr<Individual>> & group,
    const vector<float> & samples, const vector<float> & ground_truth) {

    for (int i = 0; i < group.size(); i++) {
        #pragma omp task shared(group, samples, ground_truth)
        {
            // Set each individual's fitness.
            group[i]->set_fitness(
                Evaluation::assign_rmse(
                    group[i]->get_tree()->get_rpn_string(),
                    samples, ground_truth) * group[i]->get_tree()->num_nodes()
            );
        }
    }

    #pragma omp taskwait
}

/**
 * Generate random samples for a generation's evaluation.
 * @param samples      [description]
 * @param ground_truth [description]
 * @param domain       [description]
 * @param engine       [description]
 */
void Driver::generate_samples(vector<float> & samples,
    vector<float> & ground_truth, shared_ptr<Function> func,
    uniform_real_distribution<float> & domain,
    mt19937 & engine) {
    for (int s = 0; s < Evaluation::NUM_SAMPLES; s++) {
        samples[s] = domain(engine);
        ground_truth[s] = func->call(samples[s]);
    }
}

/**
 * Evolve with OpenMP only.
 */
void Driver::evolve_openmp() {
    // Make and initialize new population.
    auto population = make_shared<Population>(this->population_size);
    population->initialize(this->root_engine, 2, 6);

    // Temporary list of vectors to pass list of individuals to be evaluated
    vector<shared_ptr<Individual>> group(population->get_length(), nullptr);

    // Construct the function we're using.
    auto func = FunctionFactory::make_function((FunctionFactory::FunctionType)this->function);

    vector<float> samples(Evaluation::NUM_SAMPLES, 0);
    vector<float> ground_truth(Evaluation::NUM_SAMPLES, 0);

    // Construct a random distribution over the function's domain.
    auto dom = func->domain();
    uniform_real_distribution<float> domain(dom.first, dom.second);

    double start_time;
    mt19937 gen_engine; // Reseeded each generation.

    // Start thread team to avoid creation and destruction at every generation.
    #pragma omp parallel
    #pragma omp single
    {
        for (int current_generation = 0; current_generation <= this->generations;
            current_generation++) {
            // Evaluate each individual in the population.
            // Generate random samples for evaluation.
            // For consistency with the hybrid version we must use a different engine.
            gen_engine.seed(this->root_engine());
            this->generate_samples(samples, ground_truth, func, domain, gen_engine);

            for (int i = 0; i < population->get_length(); i++) {
                group[i] = (*population)[i];
            }

            start_time = omp_get_wtime();
            this->evaluate_group(group, samples, ground_truth);

            // Log results of the evaluation.
            this->logger->log(population, current_generation,
                omp_get_wtime() - start_time);

            // Do evolution step: selection, crossover, and mutation.
            population->update(this->root_engine, this->crossover_rate,
                this->mutation_rate);
        }
    }
}

/**
 * Evolve with OpenMP and MPI.
 * @param rank int, process rank.
 */
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
