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
    pair<float, float> domain() {return make_pair(this->lower, this->upper);}

protected:
    Function(float _lower, float _upper) : lower(_lower), upper(_upper) {}
    float lower;
    float upper;
};

/**
 * Class representing the natural logrithm.
 */
class Log : public Function {
public:
    Log() : Function(-0.9999999, 1) {}
    float operator()(float x) {return log(1 + x);}
};

/**
 * Class representing the exponential (e^x).
 */
class Exp : public Function {
public:
    Exp() : Function(-10, 10) {}
    float operator()(float x) {return exp(x);}
};

/**
 * Class representing the sin function.
 */
class Sin : public Function {
public:
    Sin() : Function(-10, 10) {}
    float operator()(float x) {return sin(x);}
};

/**
 * Class representing a simple shifted parabola.
 */
class Parabola : public Function {
public:
    Parabola() : Function(-100, 100) {}
    float operator()(float x) {return (x + 1) * (x + 1) - 3;}
};

class FunctionFactory {
public:
    /**
     * Function types.
     */
    enum FunctionType {
        INVALID = 0,
        LOG = 1,
        EXP = 2,
        SIN = 3,
        PARABOLA = 4
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
            case EXP:
                return Exp();
                break;
            case SIN:
                return Sin();
                break;
            case PARABOLA:
                return Parabola();
                break;
            default:
                cout << "Invalid function type " << type << \
                    " exiting..." << endl;
                exit(EXIT_FAILURE);
        }
    }
};
