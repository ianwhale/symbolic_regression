#pragma once

#include<vector>
#include<random>
#include<memory>

#include "individual.h"

using std::mt19937;

typedef std::vector<shared_ptr<Individual>> pop_type;

class Population {
public:
    Population(size_t _length) : length(_length) {}

    /**
     * Randomly initialize a population with the ramped half-and-half method.
     * See:
     *  https://www.win.tue.nl/ipa/archive/falldays2007/HandoutEggermont.pdf
     * for more information on ramped half-and-half.
     * @param engine    mt19937, Mersenne Twister random engine.
     * @param max_depth int, maximum depths of all trees.
     */
    void initialize(mt19937 engine, int min_depth=2, int max_depth=6) {
        // Do one creation loop for each depth size.
        int num_initializations;
        int num_depths = max_depth - min_depth; // Number of depth categories.
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

    size_t get_length() const { return this->length; }
    indv_ptr & operator[](size_t idx) { return this->population[idx]; }

private:
    size_t length;
    pop_type population;
};
