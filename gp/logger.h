#pragma once

#include <string>
#include <memory>

using std::string;

class Population;

class Logger {
public:

    const int ARCHIVE_INTERVAL = 50;

    Logger(const string & _output_dir) : output_dir(_output_dir) {
        this->make_dir();
        this->make_unique_output_names();
    }

    void make_dir();
    void make_unique_output_names();
    void log(std::shared_ptr<Population> population, const int & current_generation,
             const double & evaluation_time);

private:
    string output_dir;      // Directory to output below files.
    string archive_name;    // Archive, stores population every so often.
    string log_name;        // Log, stores population info every generation.
};
