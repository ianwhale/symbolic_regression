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

TEST_CASE("Grow test max depth = 3", "[unit]") {
    reset_distributions();

    mt19937 engine(2);
    const int max_depth = 3;

    indv_ptr indv = Evolution::grow(max_depth, engine);

    // Just check if the individual can be evaluated.
    REQUIRE(Evaluation::evaluate_rpn(indv->get_tree()->get_rpn_string(), 3.14));
}

TEST_CASE("Grow test max depth = 6", "[unit]") {
    reset_distributions();

    mt19937 engine(123);
    const int max_depth = 6;

    indv_ptr indv = Evolution::grow(max_depth, engine);

    // Just check if the individual can be evaluated.
    REQUIRE(Evaluation::evaluate_rpn(indv->get_tree()->get_rpn_string(), 3.14));
}
