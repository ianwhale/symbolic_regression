#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "population.h"
#include "individual.h"
#include "evolution.h"
#include "logger.h"

using std::cout;
using std::endl;

using std::ofstream;
using std::vector;
using std::string;

/**
 * Log information about the current generation.
 * @param population         shared_ptr<Population>, curent population.
 * @param current_generation int, current generation.
 * @param evaluation_time    double, how long the evaluation took at current_generation.
 */
void Logger::log(std::shared_ptr<Population> population, const int & current_generation,
         const double & evaluation_time) {

     // Gather summary statistics.
     float fit_sum = 0;
     float fit_sumsq = 0; // Sum of squares.
     float fitness = 0;   // Temp variable.
     size_t n = population->get_length();

     int node_sum = 0;
     int node_sumsq = 0;
     int max_nodes = -1;
     int min_nodes = 0x7fffffff; // Max signed int.
     vector<int> nodes(population->get_length(), 0);

     for (size_t i = 0; i < population->get_length(); i++) {
         fitness = (*population)[i]->get_fitness();
         fit_sum += fitness;
         fit_sumsq += fitness * fitness;

         nodes[i] = (*population)[i]->get_tree()->num_nodes();
         node_sum += nodes[i];
         node_sumsq += nodes[i] * nodes[i];

         if (nodes[i] > max_nodes) {
             max_nodes = nodes[i];
         }
         if (nodes[i] < min_nodes) {
             min_nodes = nodes[i];
         }
     }

     float fit_stdev = n * fit_sumsq - (fit_sum * fit_sum);
     fit_stdev /= n * (n - 1);
     fit_stdev = sqrt(fit_stdev);

     float nodes_stdev = n * node_sumsq - (node_sum * node_sum);
     nodes_stdev /= (float)(n * (n - 1));
     nodes_stdev = sqrt(nodes_stdev);

     population->sort();
     std::sort(nodes.begin(), nodes.end());

     float fit_median;
     float nodes_median;
     // Even number of indiviuals.
     if (n % 2 == 0) {
         fit_median = (*population)[n / 2]->get_fitness();
         nodes_median = nodes[n / 2];
     }
     // Odd number of individuals.
     else {
         fit_median = (*population)[n / 2]->get_fitness() + (*population)[(n + 1) / 2]->get_fitness();
         fit_median /= 2;

         nodes_median = nodes[n / 2] + nodes[(n + 1) / 2];
         nodes_median /= (float)2;
     }

     // Get the best performing individual.
     indv_ptr best = (*population)[0];
     indv_ptr worst = (*population)[population->get_length() - 1];

     // Log stats about the population.
     ofstream log_file;
     log_file.open(this->log_name, std::ios::app);
     log_file << current_generation << ","
              << best->get_fitness() << ","
              << worst->get_fitness() << ","
              << fit_sum / n << ","
              << fit_stdev << ","
              << fit_median << ","
              << max_nodes << ","
              << min_nodes << ","
              << node_sum / (float)n << ","
              << nodes_stdev << ","
              << nodes_median << ","
              << node_sum << ","
              << evaluation_time << endl;
     log_file.close();

     // Archive best genome.
     ofstream archive_file;
     archive_file.open(this->archive_name, std::ios::app);
     archive_file << current_generation << ","
                  << best->get_tree()->num_nodes() << ","
                  << best->get_tree()->get_rpn_string() << ","
                  << best->get_tree()->get_infix_string() << endl;
     archive_file.close();
}

/**
 * Create the output directory if it does not exist.
 */
void Logger::make_dir() {
    struct stat sb;

    // Check if output directory exists.
    if (stat(this->output_dir.c_str(), &sb) == -1) {
        mkdir(this->output_dir.c_str(), 0775);
    }
}

/**
 * Create a unique filenames.
 */
void Logger::make_unique_output_names() {
    struct stat sb;

    string archive = this->output_dir + "/" + "archive";
    string log = this->output_dir + "/" + "log";

    int unique_id = 1;

    // Output files don't exist.
    if (access((log + ".csv").c_str(), F_OK) == -1) {
        this->archive_name = archive + ".csv";
        this->log_name = log + ".csv";
    }
    // Output files do exist, make unique names.
    else {
        while (access((log + std::to_string(unique_id) + ".csv").c_str(), F_OK) != -1) {
            unique_id++;
        }
        this->archive_name = archive + std::to_string(unique_id) + ".csv";
        this->log_name = log + std::to_string(unique_id) + ".csv";
    }

    // Write the csv header to the log file.
    string log_header =
        "generation,max_fitness,min_fitness,mean_fitness,fitness_std,median_fitness,max_nodes,min_nodes,mean_nodes,nodes_std,median_nodes,total_nodes,evaluation_time";

    ofstream log_file;
    log_file.open(this->log_name);
    log_file << log_header << endl;
    log_file.close();

    // Write the header to the archive file.
    string archive_header = "generation,best_nodes,best_genome_rpn,best_genome_infix";
    ofstream archive_file;
    archive_file.open(this->archive_name);
    archive_file << archive_header << endl;
    archive_file.close();
}
