#include<string>
#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


using std::string;
using Evaluation::is_operation;
using Evaluation::is_variable;

TEST_CASE("Test is_operation", "[unit]") {
    REQUIRE(is_operation("+"));
    REQUIRE(is_operation("-"));
    REQUIRE(is_operation("*"));
    REQUIRE(is_operation("/"));
    REQUIRE(! is_operation("x"));
    REQUIRE(! is_operation(" "));
    REQUIRE(! is_operation("3.1415"));
}


TEST_CASE("Test is_variable", "[unit]") {
    REQUIRE(is_variable("x"));
    REQUIRE(! is_variable("+"));
    REQUIRE(! is_variable("y"));
    REQUIRE(! is_variable(" "));
    REQUIRE(! is_operation("3.1415"));
}
