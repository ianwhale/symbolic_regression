#include <random>
#include "utils.h"
#include "../../gp/evolution.h"
#include "../../gp/individual.h"
#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

using std::mt19937;
using Evolution::crossover;

TEST_CASE("Test mutation on: \"5 x 7 - *\"", "[unit]") {
    reset_distributions();

    const string rpn = "5 x 7 - *";
    indv_ptr indv = make_shared<Individual>(rpn);
    mt19937 engine(0);

    // Mutation throws in a new random constant.
    const string expected = "5 x 6.885315 - * ";
    REQUIRE(Evolution::mutation(indv, engine)->get_tree()->get_rpn_string() == expected);
}

TEST_CASE("Test mutation on: \"x 4 5 + * x 8 / +\"", "[unit]") {
    reset_distributions();

    const string rpn = "x 4 5 + * x 8 / +";
    indv_ptr indv = make_shared<Individual>(rpn);
    mt19937 engine(0);

    const string expected = "x 4 5 + / x 8 / + ";
    REQUIRE(Evolution::mutation(indv, engine)->get_tree()->get_rpn_string() == expected);
}

TEST_CASE("Test mutation robustness, mutate a bunch", "[unit]") {
    reset_distributions();

    const string rpn = "x 4 5 + * x 8 / +";
    indv_ptr indv = make_shared<Individual>(rpn);
    mt19937 engine(0);

    const float test_val = 1.0;

    for (int i = 0; i < 1000; i++) {
        Evolution::mutation(indv, engine);
        // Make sure the tree isn't broken.
        REQUIRE(indv->get_tree()->num_nodes() == 9);
        Evaluation::evaluate_rpn(indv->get_tree()->get_rpn_string(), test_val);
    }
}
