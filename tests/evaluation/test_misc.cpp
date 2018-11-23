#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


TEST_CASE("Test Evaluation::is_operation", "[unit]") {
    REQUIRE(Evaluation::is_operation("+"));
    REQUIRE(Evaluation::is_operation("-"));
    REQUIRE(Evaluation::is_operation("*"));
    REQUIRE(Evaluation::is_operation("/"));
    REQUIRE(! Evaluation::is_operation("x"));
    REQUIRE(! Evaluation::is_operation(" "));
    REQUIRE(! Evaluation::is_operation("3.1415"));
    REQUIRE(! Evaluation::is_operation("-3.1415"));
}


TEST_CASE("Test Evaluation::is_variable", "[unit]") {
    REQUIRE(Evaluation::is_variable("x"));
    REQUIRE(! Evaluation::is_variable("+"));
    REQUIRE(! Evaluation::is_variable("y"));
    REQUIRE(! Evaluation::is_variable(" "));
    REQUIRE(! Evaluation::is_variable("3.1415"));
    REQUIRE(! Evaluation::is_variable("-3.1415"));
}
