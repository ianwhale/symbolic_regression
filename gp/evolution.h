#pragma once

#include <random>
#include <memory>
#include <functional>
#include "evaluation.h"
#include "individual.h"

using std::bind;
using std::make_shared;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using Evaluation::is_operation; using Evaluation::is_variable;

namespace Evolution {

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
}
