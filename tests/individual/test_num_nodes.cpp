#include <memory>
#include <string>
#include "utils.h"
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


TEST_CASE("Num nodes test.", "[unit]") {
    RPNTree tree(make_test_tree());
    REQUIRE(tree.num_nodes() == 9);
}
