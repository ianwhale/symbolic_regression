#include<vector>
#include<random>
#include<memory>

#include "evolution.h"
#include "individual.h"
#include "population.h"

using std::mt19937;


/**
 * Randomly initialize a population with the ramped half-and-half method.
 * See:
 *  https://www.win.tue.nl/ipa/archive/falldays2007/HandoutEggermont.pdf
 * for more information on ramped half-and-half.
 * @param  engine    mt19937, Mersenne Twister random generator.
 * @param  min_depth int, min tree depth.
 * @param  max_depth int, max tree depth.
 * @param  length    size_t, size of population.
 * @return           vector<indv_ptr>
 */
void Population::initialize(mt19937 & engine, int min_depth, int max_depth) {
    int num_initializations;
    int num_depths = max_depth - min_depth + 1; // Number of depth categories.
    int remainder = this->length % num_depths;

    //
    // For each depth, create half the individuals with the grow method
    // and the other half with the full method.
    //
    for (int current_depth = min_depth; current_depth <= max_depth; current_depth++) {
        num_initializations = this->length / num_depths;

        // Account for remainder.
        if (remainder > 0) {
            num_initializations++;
            remainder--;
        }

        // Initialize half the trees with "grow"
        for (int i = 0; i < num_initializations / 2; i++) {
            this->population.push_back(Evolution::grow(current_depth, engine));
        }
        // Initialize the other half with "full", accounting for remainder.
        for (int i = 0; i < (num_initializations / 2)
             + (num_initializations % 2); i++) {
            this->population.push_back(Evolution::full(current_depth, engine));
        }
    }
}

void Population::update(mt19937 engine) {}
