#include <memory>
#include <string>
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


TEST_CASE("Test Individual rpn string constructor", "[unit]") {
    const string rpn = "x 4 5 + * x 8 / + ";
    const string infix = "( x * ( 4 + 5 ) ) + ( x / 8 ) ";
    Individual indv(rpn);

    REQUIRE(indv.get_tree()->post_order() == rpn);
    REQUIRE(indv.get_tree()->in_order() == infix);
}
