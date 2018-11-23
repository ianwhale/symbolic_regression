#include<array>
#include<stack>
#include<cmath>
#include<string>
#include<vector>
#include<memory>
#include<random>
#include<sstream>
#include "function.h"
#include "evaluation.h"

#include <iostream>
using std::cout; using std::endl;

using std::string; using std::vector;
constexpr std::array<char, 4> Evaluation::OPERATIONS;

/**
 * Return a random operation not equal to the given operation.
 * @param  operation string
 * @param  engine    mt19937, Mersenne twister random engine.
 * @return           string
 */
string Evaluation::get_random_operation(string operation, std::mt19937 engine) {
    std::uniform_int_distribution<int> operation_dist(0, Evaluation::OPERATIONS.size() - 1);
    string new_operation = string(1, Evaluation::OPERATIONS[operation_dist(engine)]);

    while (new_operation == operation) {
        new_operation = Evaluation::OPERATIONS[operation_dist(engine)];
    }

    return new_operation;
}

// void print_stack(std::stack<float> s) {
//     std::stack<float> c = s; // Copy
//     cout << "Stack: ";
//     while (! c.empty()) {
//         cout << c.top() << " ";
//         c.pop();
//     }
//     cout << endl;
// }

/**
 * Evaluates the rpn string at x.
 * @param  rpn string, reverse polish notation.
 * @param  x   float
 * @return     float
 */
float Evaluation::evaluate_rpn(const string & rpn, const float & x) {
    std::stack<float> stack; // Intermediate value stack.

    std::stringstream ss(rpn);
    string current_val;

    float a, b;

    char c;
    while (getline(ss, current_val, ' ')) {
        if (is_variable(current_val) || is_operation(current_val)) {
            c = current_val[0];

            if (c == VAR) {
                stack.push(x);
            }
            else if (c == ADD) {
                get_ab(stack, a, b);
                stack.push(a + b);
            }
            else if (c == SUBTRACT) {
                get_ab(stack, a, b);
                stack.push(b - a);
            }
            else if (c == MULTIPLY) {
                get_ab(stack, a, b);
                stack.push(b * a);
            }
            else if (c == DIVIDE) {
                get_ab(stack, a, b);
                stack.push((a == 0) ? 1 : b / a); // Protected division.
            }
        }
        else {  // A consatnt value (not operator or variable).
            stack.push(stof(current_val));
        }
    }

    return stack.top();
}

/**
 * Get two values off a stack.
 * @param stack stack<float> &
 * @param a     float &
 * @param b     float &
 */
void Evaluation::get_ab(std::stack<float> & stack, float & a, float & b) {
    a = stack.top();
    stack.pop();
    b = stack.top();
    stack.pop();
}

/**
 * Tokenizes the string communicated through MPI.
 * @param  eval_string string, comma separated rpn strings.
 * @return             vector<string>
 */
vector<string> Evaluation::tokenize_comm_string(const string & rpn_string) {
    vector<string> rpn_strings;

    string temp;
    std::stringstream stream(rpn_string);

    while(getline(stream, temp, ',')) {
        rpn_strings.push_back(temp);
    }

    return rpn_strings;
}

/**
 * Exaluate a RPN string on a vector of samples in parallel.
 * @param  rpn     string, function in reverse polish notation.
 * @param  samples vector<float>, samples from the domain of a function.
 * @return         float, rmse between samples and predictions.
 */
float Evaluation::assign_rmse(const string & rpn,
                  const vector<float> & samples,
                  const vector<float> & ground_truth) {
    float rmse = 0; // Root mean squared error.

    #pragma omp parallel
    {
        float diff; // Private thread variable to store differences.
        #pragma omp for reduction(+:rmse)
        for (int i = 0; i < samples.size(); i++) {
            diff = ground_truth[i] - evaluate_rpn(rpn, samples[i]);
            rmse += diff * diff;
        }
    }

    return sqrt(rmse / samples.size());
}
