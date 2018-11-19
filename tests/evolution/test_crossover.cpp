#include <random>
#include <iostream>
#include <functional>
#include "../../gp/evolution.h"
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

using std::cout; using std::endl;

using std::mt19937;
using std::uniform_int_distribution;
using Evolution::crossover;

TEST_CASE("Test crossover \"5 x 7 - *\" and \"x 3 +\"", "[unit]") {
    mt19937 engine(0);

    const string rpn_a = "5 x 7 - *";
    const string rpn_b = "x 3 +";

    indv_ptr indv_a = make_shared<Individual>(rpn_a);
    indv_ptr indv_b = make_shared<Individual>(rpn_b);

    // Sort of uninteresting crossover, only replaces 7 with 3.
    const string expected = "5 x 3 - * ";
    indv_ptr child = crossover(indv_a, indv_b, engine);

    REQUIRE(expected == child->get_tree()->get_rpn_string());
}


TEST_CASE("Test crossover \"x 4 5 + * x 8 / +\" \"5 x 7 - *\"") {
    mt19937 engine(4);

    const string rpn_a = "x 4 5 + * x 8 / +";
    const string rpn_b = "5 x 7 - *";

    indv_ptr indv_a = make_shared<Individual>(rpn_a);
    indv_ptr indv_b = make_shared<Individual>(rpn_b);

    const string expected = "x 4 5 + * 5 x 7 - * + ";
    indv_ptr child = crossover(indv_a, indv_b, engine);

    REQUIRE(expected == child->get_tree()->get_rpn_string());
}
