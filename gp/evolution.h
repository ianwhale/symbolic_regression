#pragma once

#include <random>
#include <memory>
#include <string>
#include <functional>
#include "evaluation.h"
#include "individual.h"

using std::bind;
using std::make_shared;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;

namespace Evolution {

    uniform_real_distribution<float> EPHEMERAL_RANDOM_CONSTANTS(-10, 10);

    /**
     * Cross parent_a with parent_b. Produces one new individual.
     * @param  parent_a indv_ptr
     * @param  parent_b indv_ptr
     * @param  engine   mt19937, Mersenne twister random engine.
     * @return          indv_ptr
     */
    indv_ptr crossover(indv_ptr parent_a, indv_ptr parent_b,
                       mt19937 & engine) {
        int size_a = parent_a->get_tree()->num_nodes();
        int size_b = parent_b->get_tree()->num_nodes();

        // Copy individuals.
        indv_ptr copy_a = make_shared<Individual>(*(parent_a));
        indv_ptr copy_b = make_shared<Individual>(*(parent_b));

        // Set up distributions.
        // Subtract 2 from dist_nodes_a to disallow the root node in crossover.
        uniform_int_distribution<int> dist_nodes_a(0, size_a - 2);
        uniform_int_distribution<int> dist_nodes_b(0, size_b - 1);
        auto random_node_a = bind(dist_nodes_a, engine);
        auto random_node_b = bind(dist_nodes_b, engine);

        // Be sure not to crossover the root of the parent a.
        node_ptr cx_point_a = copy_a->node_at(random_node_a());
        node_ptr cx_point_b = copy_b->node_at(random_node_b());

        // Do crossover.
        node_ptr cx_point_a_parent = cx_point_a->parent;
        cx_point_b->parent = cx_point_a_parent;

        if (cx_point_a == cx_point_a_parent->right) {
            cx_point_a_parent->right = cx_point_b;
        }
        else {
            cx_point_a_parent->left = cx_point_b;
        }

        return copy_a;
    }

    /**
     * Carry out a point mutation on an individual.
     * @param  indv   indv_ptr
     * @param  engine mt19937, Mersenne twister random engine.
     * @return        indv_ptr, pointer to the same individual, not a copy.
     */
    indv_ptr mutation(indv_ptr indv, mt19937 & engine) {
        int size = indv->get_tree()->num_nodes();
        uniform_int_distribution<int> dist_nodes(0, size - 1);
        uniform_real_distribution<float> coin_flip(0, 1);
        node_ptr point = indv->get_tree()->node_at(dist_nodes(engine));

        // If the node is an operation, replace with an different operation.
        if (Evaluation::is_operation(point->value)) {
            point->value = Evaluation::get_random_operation(point->value, engine);
        }
        // If node is a constant or the variable, replace it with a constant
        // or the variable with 50% probability of either.
        else {
            if (coin_flip(engine) < 0.5) {
                point->value = Evaluation::VAR;
            }
            else {
                point->value = std::to_string(EPHEMERAL_RANDOM_CONSTANTS(engine));
            }
        }

        return indv;
    }
}
