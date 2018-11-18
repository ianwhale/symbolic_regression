#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif

#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <memory>
#include "omp.h"
#include "../../gp/function.h"
#include "../../gp/evaluation.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"

using std::cout; using std::endl; using std::vector;

class TestFunction : public Function {
public:
    inline float call(const float & x) const { return x + 1; }
    inline pair<float, float> domain() const { return make_pair(-100, 100); }
};


/**
 * Since we're using OpenMP, we should see a speed up with more threads.
 */
TEST_CASE("Timing", "[unit]") {
    const string rpn = "x 1 +";
    shared_ptr<Function> f = make_shared<TestFunction>();

    vector<float> samples(10000, 1);

    omp_set_num_threads(1);
    double one_thread_time = omp_get_wtime();
    Evaluation::assign_rmse(rpn, samples, f);
    one_thread_time -= omp_get_wtime();

    omp_set_num_threads(2);
    double two_threads_time = omp_get_wtime();
    Evaluation::assign_rmse(rpn, samples, f);
    two_threads_time -= omp_get_wtime();

    REQUIRE(two_threads_time > one_thread_time);
}

TEST_CASE("Correctness", "[unit]") {
    const string rpn = "x 1 +";
    shared_ptr<Function> f = make_shared<TestFunction>();

    float default_val = 1.0;

    vector<float> samples(1, default_val);
    float rmse = Evaluation::assign_rmse(rpn, samples, f);

    REQUIRE(rmse == 0);

    const string wrong_rpn = "x 2 +";
    rmse = Evaluation::assign_rmse(wrong_rpn, samples, f);

    REQUIRE(rmse == 1);
}
