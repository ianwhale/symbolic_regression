#include <set>
#include <random>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "evaluation.h"
#include "individual.h"
#include "population.h"
#include "evolution.h"

using std::make_shared;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::bernoulli_distribution;


/**
 * Cross parent_a with parent_b. Produces one new individual.
 * @param  parent_a indv_ptr
 * @param  parent_b indv_ptr
 * @param  engine   mt19937, Mersenne twister random engine.
 * @return          indv_ptr
 */
indv_ptr Evolution::crossover(indv_ptr parent_a, indv_ptr parent_b,
                   mt19937 & engine) {
    int size_a = parent_a->get_tree()->num_nodes();
    int size_b = parent_b->get_tree()->num_nodes();

    // Copy individuals.
    indv_ptr copy_a = make_shared<Individual>(*(parent_a));
    indv_ptr copy_b = make_shared<Individual>(*(parent_b));

    // Set up distributions.
    // Subtract 2 from dist_nodes_a to disallow the root node in crossover.
    uniform_int_distribution<int> random_node_a(0, size_a - 2);
    uniform_int_distribution<int> random_node_b(0, size_b - 1);

    // Be sure not to crossover the root of the parent a.
    node_ptr cx_point_a = copy_a->node_at(random_node_a(engine));
    node_ptr cx_point_b = copy_b->node_at(random_node_b(engine));

    // Do crossover.
    node_ptr cx_point_a_parent = cx_point_a->parent;
    cx_point_b->parent = cx_point_a_parent;

    if (cx_point_a == cx_point_a_parent->right) {
        cx_point_a_parent->right = cx_point_b;
    }
    else {
        cx_point_a_parent->left = cx_point_b;
    }

    if (copy_a->get_tree()->num_nodes() > MAX_NUM_NODES) {
        return make_shared<Individual>(*(parent_a));
    }
    else {
        return copy_a;
    }
}

/**
 * Carry out a point mutation on an individual.
 * @param  indv   indv_ptr
 * @param  engine mt19937, Mersenne twister random engine.
 * @return        indv_ptr, pointer to the same individual, not a copy.
 */
indv_ptr Evolution::mutation(indv_ptr indv, mt19937 & engine) {
    int size = indv->get_tree()->num_nodes();
    uniform_int_distribution<int> dist_nodes(0, size - 1);
    node_ptr point = indv->get_tree()->node_at(dist_nodes(engine));

    // If the node is an operation, replace with an different operation.
    if (Evaluation::is_operation(point->value)) {
        point->value = Evaluation::get_random_operation(point->value, engine);
    }
    // If node is a constant or the variable, replace it with a constant
    // or the variable with 50% probability of either.
    else {
        if (COIN_FLIP(engine)) {
            point->value = Evaluation::VAR;
        }
        else {
            point->value = std::to_string(EPHEMERAL_RANDOM_CONSTANTS(engine));
        }
    }

    return indv;
}

/**
 * Create an individual with the "grow" method.
 * @param  max_depth int, maximum depth of tree.
 * @param  engine    mt19937, Mersenne Twiseter random engine.
 * @return           indv_ptr
 */
indv_ptr Evolution::grow(int max_depth, mt19937 & engine) {
    return make_shared<Individual>(
        make_shared<RPNTree>(
            grow_recursion(max_depth, engine, 1, nullptr)
        )
    );
}

/**
 * "Grow" initialization recursion.
 * @param  max_depth     int
 * @param  engine        mt19937, Mersenne Twister random engine.
 * @param  current_depth int, current depth of recursion.
 * @param  parent        parent
 * @return               node_ptr, root node.
 */
node_ptr Evolution::grow_recursion(int max_depth, mt19937 & engine,
                        int current_depth, node_ptr parent) {
    node_ptr new_node;

    // Recursive case, choose between a terminal or operation node.
    if (current_depth < max_depth) {
        // There are 4 operations and 2 terminal nodes.
        // Hence, operations should have a 2/3 chance of being picked and
        // terminal nodes should only have a 1/3 chance.

        // Operation node.
        if (RAND(engine) < (2.0/3.0)) {
            new_node = make_operation(engine, parent);

            new_node->left = grow_recursion(max_depth, engine,
                current_depth + 1, new_node);
            new_node->right = grow_recursion(max_depth, engine,
                current_depth + 1, new_node);
        }
        // Terminal node.
        else {
            new_node = make_terminal(engine, parent);
        }
    }
    // Termination, must choose a terminal node.
    else {
        new_node = make_terminal(engine, parent);
    }

    return new_node;
}

/**
 * Create and individual with the "full" method.
 * @param  max_depth int, maximum depth of tree.
 * @param  engine    mt19937, Mersenne Twister random engine.
 * @return           indv_ptr
 */
indv_ptr Evolution::full(int max_depth, mt19937 & engine) {
    return make_shared<Individual>(
        make_shared<RPNTree>(
            full_recursion(max_depth, engine, 1, nullptr)
        )
    );
}

/**
 * "Full" initialization recursion.
 * @param  max_depth     int
 * @param  engine        mt19937, Mersenne Twister random engine.
 * @param  current_depth int, current depth of recursion.
 * @param  parent        parent
 * @return               node_ptr, root node.
 */
node_ptr Evolution::full_recursion(int max_depth, mt19937 & engine,
                        int current_depth, node_ptr parent) {
    node_ptr new_node;

    // Recursive case, intialize both subtrees with full method.
    if (current_depth < max_depth) {
        new_node = make_operation(engine, parent);

        new_node->left = full_recursion(max_depth, engine,
            current_depth + 1, new_node);
        new_node->right = full_recursion(max_depth, engine,
            current_depth + 1, new_node);
    }
    // Termination, we're at the max depth.
    else {
        new_node = make_terminal(engine, parent);
    }

    return new_node;
}

/**
 * Make an operation node.
 * @param  engine mt19937, Mersenne Twister random generator.
 * @param  parent node_ptr
 * @return        node_ptr
 */
node_ptr Evolution::make_operation(mt19937 & engine, node_ptr parent) {
    node_ptr node = make_shared<RPNNode>();
    node->value = Evaluation::get_random_operation("", engine);
    node->parent = parent;

    return node;
}

/**
 * Make a terminal node.
 * @param  engine mt19937, Mersenne Twister random generator.
 * @param  parent node_ptr
 * @return        node_ptr
 */
node_ptr Evolution::make_terminal(mt19937 & engine, node_ptr parent) {
    node_ptr node = make_shared<RPNNode>();
    node->parent = parent;

    // Use the variable as the terminal.
    if (COIN_FLIP(engine)) {
        node->value = Evaluation::VAR;
    }
    // Use an ephemeral random constant as the terminal.
    else {
        node->value = std::to_string(EPHEMERAL_RANDOM_CONSTANTS(engine));
    }

    return node;
}

/**
 * Select an individual from the population.
 * @param  population      shared_ptr<Population>
 * @param  tournament_size size_t, number of individuals in tournament.
 * @return                 indv_ptr, winner of tournament.
 */
indv_ptr Evolution::tournament_selection(Population * population,
                                         mt19937 & engine,
                                         size_t tournament_size) {
    std::set<size_t> unique_indices;
    uniform_int_distribution<size_t> random_indv(0, population->get_length() - 1);

    // Chose random individuals.
    while(unique_indices.size() < tournament_size) {
        unique_indices.insert(random_indv(engine));
    }

    // Perform tournament (pick individual with best fitness).
    float best_fitness = HUGE_VALF;
    float current_fitness;
    indv_ptr winner = nullptr;
    for (size_t index : unique_indices) {

        current_fitness = (*population)[index]->get_fitness();

        if (current_fitness < best_fitness) {
            best_fitness = current_fitness;
            winner = (*population)[index];
        }
    }

    return winner;
}
