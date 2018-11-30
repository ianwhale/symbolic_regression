#pragma once

#include<vector>
#include<random>
#include<memory>

using std::mt19937;

class Individual;
typedef std::shared_ptr<Individual> indv_ptr;
typedef std::vector<indv_ptr> pop_type;

class Population {
public:
    const int TOURNAMENT_SIZE = 3;

    Population(size_t _length) : length(_length) {}

    void initialize(mt19937 & engine, int min_depth, int max_depth);
    void update(mt19937 & engine, const float & crossover_rate,
        const float & mutation_rate);

    size_t get_length() const { return this->population.size(); }
    indv_ptr & operator[](const size_t & idx) { return this->population[idx]; }
    void sort();

private:
    size_t length;
    pop_type population;
};
