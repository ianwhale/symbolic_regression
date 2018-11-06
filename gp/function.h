#pragma once

#include<memory>
#include<vector>
#include<math.h>
#include<iostream>
#include<stdlib.h>
#include<utility>

using namespace std;

/**
 * Abstract base class for functions.
 */
class Function {
public:
    virtual float operator()(float x);
    virtual pair<float, float> domain();
};

/**
 * Class representing the natural logrithm.
 */
class Log : public Function {
public:
    float operator()(float x) {return log(1 + x);}
    pair<float, float> domain() {return make_pair(-1.000001, 1000.0);}
};

/**
 * Class representing sqrt.
 */
class Sqrt : public Function {
public:
    float operator()(float x) {return sqrt(1 + x);}
    pair<float, float> domain() {return make_pair(-1.0, 1000.0);}
};

/**
 * Class representing the sin function.
 */
class Sin : public Function {
public:
    float operator()(float x) {return sin(x);}
    pair<float, float> domain() {return make_pair(-10, 10);}
};

class FunctionFactory {
public:
    enum FunctionType {
        INVALID = 0,
        LOG = 1,
        SQRT = 2,
        SIN = 3
    };

    /**
     * Make function from integer description.
     * @param const int type, one of the valid FunctionTypes.
     * @return Function
     */
    static Function makeFunction(const int type) {
        switch (type) {
            case LOG:
                return Log();
                break;
            case SQRT:
                return Sqrt();
                break;
            case SIN:
                return Sin();
                break;
            default:
                cout << "Invalid function type " << type << \
                    " exiting..." << endl;
                exit(EXIT_FAILURE);
        }
    }
};
