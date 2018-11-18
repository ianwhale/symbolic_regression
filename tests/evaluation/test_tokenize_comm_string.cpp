#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif

#include<string>
#include<vector>
#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


using std::string; using std::vector;

TEST_CASE("Test tokenizing", "[unit]") {
    const string test_str = "x 3 + x *,something else,";

    vector<string> ans = {
        "x 3 + x *",
        "something else"
    };

    vector<string> tokenized = Evaluation::tokenize_comm_string(test_str);

    for (int i = 0; i < ans.size(); i++) {
        REQUIRE(ans[i] == tokenized[i]);
    }
}
