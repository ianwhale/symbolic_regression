#include <string>
#include <cmath>
#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

#define EPSILON 1e-6

#include<iostream>

using namespace std;

float abs_diff(float a, float b) { return abs(a - b); }

/**
 * Test the formula: x * (x + 3)
 */
TEST_CASE("RPN String: \"x 3 + x * \"", "[unit]") {
    const string rpn = "x 3 + x *";

    REQUIRE(Evaluation::evaluate_rpn(rpn, 1.0f) == 4.0f);
    REQUIRE(Evaluation::evaluate_rpn(rpn, -1.0f) == -2.0f);
    REQUIRE(Evaluation::evaluate_rpn(rpn, 0.0f) == 0.0f);
}

/**
 * Test the formula: 1 / x
 */
TEST_CASE("RPN String: \"1 x /\"", "[unit]") {
    const string rpn = "1 x /";

    REQUIRE(Evaluation::evaluate_rpn(rpn, 0.0f) == 1.0f);
    REQUIRE(Evaluation::evaluate_rpn(rpn, 1.0f) == 1.0f);
    REQUIRE(Evaluation::evaluate_rpn(rpn, 5.0f) == 0.2f);
}

/**
 * Test the formula: x * (1 + (2 * x)) - (10 / x)
 */
TEST_CASE("RPN String: \"x 1 2 x * + * 10 x / -\"", "[unit]") {
    const string rpn = "x 1 2 x * + * 10 x / -";

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, 1.0f), -7.0f) < EPSILON
    );
    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, 1.5f), -(2.0/3.0)) < EPSILON
    );
}

/**
 * Test the formula: x - (x * x) / 2 + (x * x * x) / 3
 */
TEST_CASE("RPN String: \"x x x * 2 / - x x * x * 3 / +\"", "[unit]") {
    const string rpn = "x x x * 2 / - x x * x * 3 / +";

    REQUIRE(
          abs_diff(Evaluation::evaluate_rpn(rpn, 1.0f), (5.0/6.0)) < EPSILON
    );

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, (5.0/4.0)), (215.0/192.0)) < EPSILON
    );

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, -(17.0/9.0)), -(25891.0/4374.0)) < EPSILON
    );
}

/**
 * Test formula: x * 1.25
 */
TEST_CASE("RPN String: \"x 1.25 *\"", "[unit]") {
    const string rpn = "x 1.25 *";

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, (2.0/3.0)), (5.0/6.0)) < EPSILON
    );

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, (5.0/8.0)), (25.0/32.0)) < EPSILON
    );
}


/**
 * Weird test case: x^3 + 2 x ^ 2 - 0.00001x - 2.00001
 */
TEST_CASE("Big RPN.", "[unit]") {
    const string rpn = "x x x * -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 + * -1.018037 + + -1.018037 + -1.018037 -1.018037 -1.018037 + + + * + + -1.018037 + -1.018037 -1.018037 + x x -1.018037 + + + + + * -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 -1.018037 + * -1.018037 + + -1.018037 + -1.018037 -1.018037 -1.018037 + + + * + + -1.018037 + -1.018037 -1.018037 + x x -1.018037 + + + + + ";

    REQUIRE(
        abs_diff(Evaluation::evaluate_rpn(rpn, -1.5), -0.874994) < EPSILON
    );
}
