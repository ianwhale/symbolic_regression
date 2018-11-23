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
    Population(size_t _length) : length(_length) {}

    void initialize(mt19937 & engine, int min_depth, int max_depth);
    void update(mt19937 engine);

    size_t get_length() const { return this->population.size(); }
    indv_ptr & operator[](const size_t & idx) { return this->population[idx]; }

private:
    size_t length;
    pop_type population;
};
