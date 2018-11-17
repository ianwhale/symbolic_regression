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
    virtual float call(const float & x) const = 0;
    virtual pair<float, float> domain() const = 0;

};

/**
 * Class representing the natural logrithm.
 */
class Log : public Function {
public:
    inline float call(const float & x) const { return log(1 + x); }
    inline pair<float, float> domain() const { return make_pair(-0.99999, 1); }
};

/**
 * Class representing the exponential (e^x).
 */
class Exp : public Function {
public:
    inline float call(const float & x) const { return exp(x); }
    inline pair<float, float> domain() const { return make_pair(-10, 10); }
};

/**
 * Class representing the sin function.
 */
class Sin : public Function {
public:
    inline float call(const float & x) const { return sin(x); }
    inline pair<float, float> domain() const { return make_pair(-10, 10); }
};

/**
 * Class representing a simple shifted parabola.
 */
class Parabola : public Function {
public:
    inline float call(const float & x) const { return (x + 1) * (x + 1) - 3; }
    inline pair<float, float> domain() const { return make_pair(-100, 100); }
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
    static shared_ptr<Function> makeFunction(const int type) {
        switch (type) {
            case LOG:
                return make_shared<Log>();
                break;
            case EXP:
                return make_shared<Exp>();
                break;
            case SIN:
                return make_shared<Sin>();
                break;
            case PARABOLA:
                return make_shared<Parabola>();
                break;
            default:
                cout << "Invalid function type " << type << \
                    " exiting..." << endl;
                exit(EXIT_FAILURE);
        }
    }
};
