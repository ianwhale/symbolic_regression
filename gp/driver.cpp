#include <random>
#include <memory>
#include <vector>
#include <sstream>
#include <fstream>
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
    this->logger = new Logger(_output_dir, _seed);
}


/**
 * Evaluates a group of individuals.
 * @param group vector of pointers to individuals.
 */
void Driver::evaluate_population(shared_ptr<Population> population,
    const vector<float> & samples, const vector<float> & ground_truth) {

    for (int i = 0; i < population->get_length(); i++) {
        #pragma omp task shared(population, samples, ground_truth)
        {
            // Set each individual's fitness.
            (*population)[i]->set_fitness(
                Evaluation::assign_rmse(
                    (*population)[i]->get_tree()->get_rpn_string(),
                    samples, ground_truth) * (*population)[i]->get_tree()->num_nodes()
            );
        }
    }

    #pragma omp taskwait
}


/**
 * Evaluate a group of strings rather than individuals.
 * @param rpn_strings  vector<string>
 * @param samples      vector<float>, random samples from domain
 * @param ground_truth vector<float>, function applied to samples
 * @param fitnesses    vector<float>, results of RMSE calculation
 */
void evaluate_group_strings(const vector<string> & rpn_strings,
            const vector<float> & samples, const vector<float> & ground_truth,
            vector<float> & fitnesses) {
    for (int i = 0; i < rpn_strings.size(); i++) {
        #pragma omp task shared(rpn_strings, samples, ground_truth, fitnesses)
        {
            // Get number of nodes.
            std::stringstream ss(rpn_strings[i]);
            string temp;
            int count = 0;
            while(getline(ss, temp, ' ')) { count++; }

            // Set each individual's fitness.
            fitnesses[i] = Evaluation::assign_rmse(rpn_strings[i],
                samples, ground_truth) * count;;
        }
    }

    #pragma omp taskwait
}


/**
 * Translates indices for assigning fitnesses from distributed evaluation.
 * @param  rank               int, rank being considered.
 * @param  index_in_fitnesses int, index in array sent from evaluation.
 * @param  indvs_per_rank     vector<int>, number of individuals sent to each rank.
 * @return                    int, translated index.
 */
int translate_index(int rank, int index_in_fitnesses,
        const vector<int> & indvs_per_rank) {
    // Rank 0 doesn't need to translate.
    if (rank == 0) {
        return index_in_fitnesses;
    }
    // Shifts index by how many individuals came before it in the population.
    else {
        int previous_indvs = 0;
        for (int i = rank - 1; i < rank; i++) {
            previous_indvs += indvs_per_rank[i];
        }
        return previous_indvs + index_in_fitnesses;
    }
}

/**
 * Generate random samples for a generation's evaluation.
 * @param samples      vector<float>, size of Evaluation::NUM_SAMPLES
 * @param ground_truth vector<float>, size of Evaluation::NUM_SAMPLES
 * @param domain       uniform_real_distribution<float>, domain of function.
 * @param engine       mt19937, Meresenne Twister random engine.
 */
void Driver::generate_samples(vector<float> & samples,
    vector<float> & ground_truth, shared_ptr<Function> func,
    uniform_real_distribution<float> & domain,
    mt19937 & engine) {
    domain.reset();
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
    this->logger->initialize();

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
            auto seed = this->root_engine();
            gen_engine.seed(seed);

            this->generate_samples(samples, ground_truth, func, domain, gen_engine);

            start_time = omp_get_wtime();
            this->evaluate_population(population, samples, ground_truth);

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
 * Register incoming and outgoing datatypes.
 * @param Outgoing_DT MPI_Datatype pointer
 * @param Incoming_DT MPI_Datatype pointer
 */
void register_datatypes(MPI_Datatype * Outgoing_DT) {
    MPI_Datatype outgoing_types[2] = {MPI_INT, MPI_INT};
    int block_lengths[2] = {1, 1};
    MPI_Aint outgoing_displacements[2] = {offsetof(OutgoingPayload, seed),
        offsetof(OutgoingPayload, payload_length)};

    MPI_Type_create_struct(2, block_lengths, outgoing_displacements,
        outgoing_types, Outgoing_DT);
    MPI_Type_commit(Outgoing_DT);
}

/**
 * Make payloads and store relevant information in outgoing pointer.
 * @param  payloads       vector<string>, reverse polish strings to evaluate.
 * @param  indvs_per_rank vector<int>, how many individuals per rank.
 * @param  population     shared_ptr<Population>
 * @param  outgoing       OutgoingPayload, pointer, updated after constructing.
 */
void make_payloads(vector<string> & payloads, const vector<int> & indvs_per_rank,
    shared_ptr<Population> population, OutgoingPayload * outgoing,
    mt19937 & root_engine) {
    int max_payload_length = 0;
    int previous_start = 0;

    // Build the strings to send to each rank.
    for (int i = 0; i < indvs_per_rank.size(); i++) {
        string payload = "";

        //
        // Each payload is a comma seperated string that contains
        // each reverse polish string that must be evaluated.
        //
        for (int j = 0; j < indvs_per_rank[i]; j++) {
            payload += (*population)[j + previous_start]->get_tree()->get_rpn_string() + ",";
        }

        // cout << "Payload length: " << payload.length() << endl;

        if (payload.length() > max_payload_length) {
            max_payload_length = payload.length();
        }
        payloads[i] = payload + '\0';
        previous_start += indvs_per_rank[i];
    }

    outgoing->seed = root_engine(); // Make a random seed to send to each rank.
    outgoing->payload_length = max_payload_length + 1; // Each rank allocates enough space for the largest payload.

}

/**
 * Evolve with OpenMP and MPI.
 * @param rank int, process rank.
 * @param size int, number of ranks.
 */
void Driver::evolve_hybrid(const int & rank, const int & size) {
    MPI_Datatype Outgoing_DT;
    register_datatypes(&Outgoing_DT);

    const int MASTER_TO_SLAVE_TAG = 0;
    const int SLAVE_TO_MASTER_TAG = 1;

    mt19937 gen_engine; // Each rank has an engine to generate samples.
                        // This will be seeded each evaluation for consistency.

    // Make sure everyone has the function we're using.
    auto func = FunctionFactory::make_function((FunctionFactory::FunctionType)this->function);

    // Initialize space for the samples and ground truth on each rank.
    vector<float> samples(Evaluation::NUM_SAMPLES, 0);
    vector<float> ground_truth(Evaluation::NUM_SAMPLES, 0);

    // Construct a random distribution over the function's domain on each rank.
    auto dom = func->domain();
    uniform_real_distribution<float> domain(dom.first, dom.second);

    vector<int> indvs_per_rank(size, this->population_size / size);
    vector<string> payloads(size);
    shared_ptr<Population> population;

    // Only initialize the population on the master rank.
    if (rank == this->MASTER) {
        this->logger->initialize();

        population = make_shared<Population>(this->population_size);
        population->initialize(this->root_engine, 2, 6);

        // Determine how many individuals each rank gets.
        int remainder = this->population_size % size;
        for (int i = 0; i < size && remainder > 0; i++) {
            indvs_per_rank[i]++;
            remainder--;

        }
    }
    OutgoingPayload outgoing; // Root's outgoing payload.

    string current_val; // Temp variable.
    double start_time;

    #pragma omp parallel
    #pragma omp single
    {
        for (int current_generation = 0; current_generation <= this->generations;
            current_generation++) {

            // Only the master process performs the main evolution loop.
            if (rank == this->MASTER) {
                make_payloads(payloads, indvs_per_rank, population, &outgoing,
                              this->root_engine);
            }

            MPI_Bcast(&outgoing, 1, Outgoing_DT, this->MASTER, MPI_COMM_WORLD);

            // Allocate space for the string to evaluate.
            char string_to_eval[outgoing.payload_length];

            MPI_Request ignore;
            if (rank == this->MASTER) {
                // Send out all the strings (even to self)
                start_time = omp_get_wtime();
                for (int i = 0; i < size; i++) {
                    // cout << "Rank " << i << " gets payload with length " << payloads[i].length() << " (" << indvs_per_rank[i] << " individuals)" << endl;
                    MPI_Isend(payloads[i].c_str(), payloads[i].length(), MPI_CHAR, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &ignore);
                }
            }

            // cout << "Recieving evaluation string on rank " << rank << endl;
            MPI_Recv(string_to_eval, outgoing.payload_length, MPI_CHAR, this->MASTER, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string evaluate_me(string_to_eval);
            std::stringstream ss(string_to_eval);
            vector<string> group;

            gen_engine.seed(outgoing.seed);

            this->generate_samples(samples, ground_truth, func, domain, gen_engine);

            // Evaluate all the RPN strings.
            while(getline(ss, current_val, ',')) {
                group.push_back(current_val);
            }

            vector<float> fitnesses(group.size(), 0);
            evaluate_group_strings(group, samples, ground_truth, fitnesses);

            // for (int i = 0; i < fitnesses.size(); i++) {
            //     cout << "(Rank " << rank << "): fitnesses[" << i << "] = " << fitnesses[i] << endl;
            // }

            // cout << "Rank " << rank << " sending " << fitnesses.size() << " fitnesses" << endl;
            MPI_Isend(&fitnesses[0], fitnesses.size(), MPI_FLOAT, this->MASTER, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &ignore);

            if (rank == this->MASTER) {
                for (int i = 0; i < size; i++) {
                    float fits_from_rank[indvs_per_rank[i]];

                    // cout << "Recieving " << indvs_per_rank[i] <<  " fitnesses from rank " << i << endl;

                    MPI_Recv(fits_from_rank, indvs_per_rank[i], MPI_FLOAT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = 0; j < indvs_per_rank[i]; j++) {
                        (*population)[translate_index(i, j, indvs_per_rank)]
                            ->set_fitness(fits_from_rank[j]);
                    }
                }

                // Log results of evaluation and do population update.
                this->logger->log(population, current_generation,
                    omp_get_wtime() - start_time);

                // Do evolution step.
                population->update(this->root_engine, this->crossover_rate,
                    this->mutation_rate);
            }
        }
    }
}

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
        this->evolve_hybrid(rank, size);
    }
    // One rank, use OpenMP only.
    else {
        MPI_Finalize(); // Don't need MPI anymore.
        this->evolve_openmp();
        return; // Avoid calling MPI_Finalize twice.
    }

    MPI_Finalize();
}
