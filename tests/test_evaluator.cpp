#define CATCH_CONFIG_MAIN
#undef NDEBUG

#include "../third-party/Catch2/single_include/catch2/catch.hpp"

#include <iostream>
#include <string>
#include <cmath>
#include "../gp/evaluator.h"

using std::cout; using std::endl;

bool float_eq(float a, float b) {return abs(a - b) < 1e-8;}

/**
 * Test the formula: x * (x + 3)
 */
TEST_CASE("RPN String: \"x 3 + x * \"", "[unit]") {
    const string rpn = "x 3 + x *";

    REQUIRE(Evaluator::evaluate_rpn(rpn, 1.0f) == 4.0f);
    REQUIRE(Evaluator::evaluate_rpn(rpn, -1.0f) == -2.0f);
    REQUIRE(Evaluator::evaluate_rpn(rpn, 0.0f) == 0.0f);
}

/**
 * Test the formula: 1 / x
 */
TEST_CASE("RPN String: \"1 x /\"", "[unit]") {
    const string rpn = "1 x /";

    REQUIRE(Evaluator::evaluate_rpn(rpn, 0.0f) == 1.0f);
    REQUIRE(Evaluator::evaluate_rpn(rpn, 1.0f) == 1.0f);
    REQUIRE(Evaluator::evaluate_rpn(rpn, 5.0f) == 0.2f);
}

/**
 * Test the formula: x * (1 + (2 * x)) - (10 / x)
 */
TEST_CASE("RPN String: \"x 1 2 x * + * 10 x / -\"", "[unit]") {
    const string rpn = "x 1 2 x * + * 10 x / -";

    REQUIRE(
        float_eq(Evaluator::evaluate_rpn(rpn, 1.0f), -7.0f)
    );
    REQUIRE(
        float_eq(Evaluator::evaluate_rpn(rpn, 1.5f), -(2/3))
    );
}

/**
 * Test the formula: x - (x * x) / 2 + (x * x * x) / 3
 */
TEST_CASE("RPN String: \"x x x * 2 / - x x * x * 3 / +\"", "[unit]") {
    const string rpn = "x x x * 2 / - x x * x * 3 / +";

    REQUIRE(
        float_eq(Evaluator::evaluate_rpn(rpn, 5.6), 48.4587)
    );

    REQUIRE(
        float_eq(Evaluator::evaluate_rpn(rpn, -1.449), -3.51291)
    );
}
