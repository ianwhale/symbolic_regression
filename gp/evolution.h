#pragma once

#include <memory>

using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::bernoulli_distribution;

class Population; class Individual; class RPNNode;

typedef std::shared_ptr<RPNNode> node_ptr;
typedef std::shared_ptr<Individual> indv_ptr;

struct Evolution {
    static double COIN_FLIP(mt19937 engine) {
        bernoulli_distribution cf(0.5);
        return cf(engine);
    }

    static double EPHEMERAL_RANDOM_CONSTANTS(mt19937 engine) {
        uniform_real_distribution<float> erc(-10, 10);
        return erc(engine);
    }

    static double RAND(mt19937 engine) {
        uniform_real_distribution<float> r(0, 1);
        return r(engine);
    }

    static const int MAX_NUM_NODES = 4096; // 2^12.

    static indv_ptr crossover(indv_ptr parent_a, indv_ptr parent_b,
                       mt19937 & engine);
    static indv_ptr mutation(indv_ptr indv, mt19937 & engine);
    static indv_ptr grow(int max_depth, mt19937 & engine);
    static node_ptr grow_recursion(int max_depth, mt19937 & engine,
                            int current_depth, node_ptr parent);
    static indv_ptr full(int max_depth, mt19937 & engine);
    static node_ptr full_recursion(int max_depth, mt19937 & engine,
                            int current_depth, node_ptr parent);
    static node_ptr make_operation(mt19937 & engine, node_ptr parent);
    static node_ptr make_terminal(mt19937 & engine, node_ptr parent);
    static indv_ptr tournament_selection(Population * population,
                                             mt19937 & engine,
                                             size_t tournament_size);
private:
    Evolution() {}
};
