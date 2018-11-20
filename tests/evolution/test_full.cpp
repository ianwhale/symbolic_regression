#include <random>
#include "utils.h"
#include "../../gp/evolution.h"
#include "../../gp/evaluation.h"
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

#include<iostream>
using std::cout; using std::endl;
using std::bernoulli_distribution;
using std::uniform_int_distribution;

using std::mt19937;

/**
 * Enforce constraits of the "full" initialization method.
 * @param current [description]
 */
void require_full_constraints(node_ptr current) {
    if (current->is_leaf()) {
        REQUIRE(! Evaluation::is_operation(current->value));
    }
    else {
        REQUIRE(Evaluation::is_operation(current->value));
        REQUIRE(current->left != nullptr);
        REQUIRE(current->right != nullptr);

        require_full_constraints(current->left);
        require_full_constraints(current->right);
    }
}

TEST_CASE("Full test max depth = 3", "[unit]") {
    reset_distributions();

    mt19937 engine(2);
    const int max_depth = 3;

    indv_ptr indv = Evolution::full(max_depth, engine);
    require_full_constraints(indv->get_tree()->get_root());
}

TEST_CASE("Full test max depth = 6", "[unit]") {
    reset_distributions();

    mt19937 engine(123);
    const int max_depth = 6;

    indv_ptr indv = Evolution::full(max_depth, engine);
    require_full_constraints(indv->get_tree()->get_root());
}
