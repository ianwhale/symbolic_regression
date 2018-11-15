#pragma once

#include<stack>
#include<string>
#include<map>
#include<cmath>

using std::string;

/**
 * Static class for handling evaluation.
 */
namespace Evaluation {
    float evaluate_rpn(const string & rpn, const float & x);
    void getAB(std::stack<float> & stack, float & a, float & b);

    // Operation constants.
    const string VAR = "x";
    const string ADD = "+";
    const string SUBTRACT = "-";
    const string MULTIPLY = "*";
    const string DIVIDE = "/";

    /**
     * Evaluates the rpn string at x.
     * @param  rpn string, reverse polish notation.
     * @param  x   float
     * @return     float
     */
    float evaluate_rpn(const string & rpn, const float & x) {
        std::stack<float> stack; // Intermediate value stack.

        std::stringstream ss(rpn);
        string current_val;

        float a, b;

        while (getline(ss, current_val, ' ')) {
            if (current_val == VAR) {
                stack.push(x);
            }
            else if (current_val == ADD) {
                getAB(stack, a, b);
                stack.push(a + b);
            }
            else if (current_val == SUBTRACT) {
                getAB(stack, a, b);
                stack.push(b - a);
            }
            else if (current_val == MULTIPLY) {
                getAB(stack, a, b);
                stack.push(b * a);
            }
            else if (current_val == DIVIDE) {
                getAB(stack, a, b);
                stack.push((a == 0) ? 1 : b / a); // Protected division.
            }
            else {  // A consatnt value (not operator or variable).
                stack.push(stof(current_val));
            }
        }

        return stack.top();
    }

    /**
     * Get two values off a stack.
     * @param stack stack<float> *
     * @param a     float &
     * @param b     float &
     */
    void getAB(std::stack<float> & stack, float & a, float & b) {
        a = stack.top();
        stack.pop();
        b = stack.top();
        stack.pop();
    }

}
