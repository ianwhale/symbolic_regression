#pragma once

#include<stack>
#include<string>
#include<map>

using std::string;

/**
 * Static class for handling evaluation.
 */
class Evaluator {
public:
    /**
     * Evaluates the rpn string at x.
     * @param  rpn string, reverse polish notation.
     * @param  x   float
     * @return     float
     */
    static float evaluate_rpn(const string & rpn, float x) {
        std::stack<float> stack; // Intermediate value stack.

        const string VAR = "x";
        const string ADD = "+";
        const string SUBTRACT = "-";
        const string MULTIPLY = "*";
        const string DIVIDE = "/";

        std::stringstream ss(rpn);
        string current_val;

        float a, b;

        while (getline(ss, current_val, ' ')) {
            if (current_val == VAR) {
                stack.push(x);
            }
            else if (current_val == ADD) {
                Evaluator::getAB(&stack, a, b);
                stack.push(a + b);
            }
            else if (current_val == SUBTRACT) {
                Evaluator::getAB(&stack, a, b);
                stack.push(b - a);
            }
            else if (current_val == MULTIPLY) {
                Evaluator::getAB(&stack, a, b);
                stack.push(b * a);
            }
            else if (current_val == DIVIDE) {
                Evaluator::getAB(&stack, a, b);
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
    static void getAB(std::stack<float> * stack, float & a, float & b) {
        a = stack->top();
        stack->pop();
        b = stack->top();
        stack->pop();
    }
};
