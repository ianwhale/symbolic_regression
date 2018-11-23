#pragma once

#include<string>
#include<stack>
#include<random>
#include<array>

using std::string; using std::vector;

/**
 * struct for handling evaluation.
 */
struct Evaluation {
    // Operation constants.
    static const char VAR = 'x';
    static const char ADD = '+';
    static const char SUBTRACT = '-';
    static const char MULTIPLY = '*';
    static const char DIVIDE = '/';

    // Number of samples taken from the domain of each function.
    static const size_t NUM_SAMPLES = 100;

    // Constant to store the operation set.
    static constexpr std::array<char, 4> OPERATIONS = {'+', '-', '*', '/'};

    static string get_random_operation(string operation, std::mt19937 engine);
    static float evaluate_rpn(const string & rpn, const float & x);
    static void get_ab(std::stack<float> & stack, float & a, float & b);
    static vector<string> tokenize_comm_string(const string & rpn_string);
    static float assign_rmse(const string & rpn,
                      const vector<float> & samples,
                      const vector<float> & ground_truth);

    /**
     * Determine if a string is an operation.
     * @param  s string
     * @return   bool, true if s is an operation.
     */
    static bool is_operation(const string & s) {
        char c = s[0];
        return (s.length() == 1)
            && (c == ADD || c == SUBTRACT || c == MULTIPLY || c == DIVIDE);
    }

    /**
     * Determine if a string represents a variable.
     * @param  s string
     * @return   bool, true if s is a variable.
     */
    static bool is_variable(const string & s) {
        char c = s[0];
        return (s.length() == 1) && (c == VAR);
    }

private:
    Evaluation() {}
};
