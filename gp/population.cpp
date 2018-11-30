#include<cmath>
#include<vector>
#include<random>
#include<memory>
#include<algorithm>

#include "evolution.h"
#include "individual.h"
#include "population.h"

using std::mt19937;
using std::shared_ptr;
using std::make_shared;
using std::vector;

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

/**
 * Sorts the population.
 */
void Population::sort() {
    std::sort(this->population.begin(), this->population.end(),
        [](const indv_ptr & a, const indv_ptr & b) -> bool
        {
            return a->get_fitness() > b->get_fitness();
        });
}

/**
 * Update the population based on fitness. This is the reproduction step.
 * @param engine mt19937
 */
void Population::update(mt19937 & engine, const float & crossover_rate,
        const float & mutation_rate) {
    float best_fitness = HUGE_VALF;
    size_t best_index = 0;
    vector<indv_ptr> new_population;

    // We want an elitism of 1, so find the best individual and save it.
    for (size_t i = 0; i < this->length; i++) {
        if (this->population[i]->get_fitness() < best_fitness) {
            best_fitness = this->population[i]->get_fitness();
            best_index = i;
        }
    }

    new_population.push_back(this->population[best_index]);

    indv_ptr parent_a = nullptr;
    indv_ptr parent_b = nullptr;
    indv_ptr child = nullptr;

    // Loop minus 1 to account for elitism.
    for (size_t i = 0; i < this->length - 1; i++) {
        parent_a = Evolution::tournament_selection(this, engine,
            this->TOURNAMENT_SIZE);

        while (parent_b == nullptr || parent_a == parent_b) {
            parent_b = Evolution::tournament_selection(this, engine,
                this->TOURNAMENT_SIZE);
        }

        // Should we do crossover?
        if (Evolution::RAND(engine) < crossover_rate) {
            child = Evolution::crossover(parent_a, parent_b, engine);
        }
        else { // If not, make a copy.
            child = make_shared<Individual>(*parent_a);
        }

        // Should we do mutation?
        if (Evolution::RAND(engine) < mutation_rate) {
            child = Evolution::mutation(child, engine);
        }

        new_population.push_back(child);
    }

    // Swap in the new pointers.
    for (size_t i = 0; i < this->length; i++) {
        this->population[i] = new_population[i];
    }
}
