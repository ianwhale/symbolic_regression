#pragma once

#include <string>
#include <memory>

using std::string;

class Population;

class Logger {
public:
    Logger(const string & _output_dir) : output_dir(_output_dir) {}
    void log(std::shared_ptr<Population> population, const int & current_generation,
             const double & evaluation_time);

private:
    string output_dir;
};
