#include <random>
#include "../evolution/utils.h"
#include "../../gp/population.h"
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

using std::mt19937;

TEST_CASE("Test initialize", "[unit]") {
    reset_distributions();

    size_t len = 50;
    int min_depth = 2;
    int max_depth = 6;
    mt19937 engine(0);
    Population pop(len);
    pop.initialize(engine, min_depth, max_depth);

    for (int i; i < pop.get_length(); i++) {
        REQUIRE(get_depth(pop[i]->get_tree()->get_root()) <= max_depth);
    }
}
