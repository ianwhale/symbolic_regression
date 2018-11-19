#include <memory>
#include <string>
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

#include <iostream>
using std::cout; using std::endl;

using std::string;


TEST_CASE("Test get node: x 3 + ", "[unit]") {
    const string rpn = "x 3 +";
    Individual indv(rpn);

    REQUIRE(indv.node_at(0)->value == "x");
    REQUIRE(indv.node_at(1)->value == "3");
    REQUIRE(indv.node_at(2)->value == "+");
}

TEST_CASE("Test get node: ", "[unit]") {
    const string rpn = "x 4 5 + * x 8 / + ";
    Individual indv(rpn);

    REQUIRE(indv.node_at(0)->value == "x");
    REQUIRE(indv.node_at(1)->value == "4");
    REQUIRE(indv.node_at(2)->value == "5");
    REQUIRE(indv.node_at(3)->value == "+");
    REQUIRE(indv.node_at(4)->value == "*");
    REQUIRE(indv.node_at(5)->value == "x");
    REQUIRE(indv.node_at(6)->value == "8");
    REQUIRE(indv.node_at(7)->value == "/");
    REQUIRE(indv.node_at(8)->value == "+");
}
