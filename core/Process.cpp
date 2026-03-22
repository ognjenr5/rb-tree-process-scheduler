#include <sstream>

#include "Process.h"

Process::Process(const std::string& name, const int complete_time, const int max_wait_time) :
    name(name),
    complete_time(complete_time),
    max_wait_time(max_wait_time),
    curr_wait_time(0),
    curr_exec_time(0),
    containing_node(nullptr) {}

int Process::execute_process(const int time_slice) {
    const int remaining_time = this->complete_time - this->curr_exec_time;
    if (remaining_time < time_slice) return remaining_time;

    this->curr_exec_time += time_slice;
    this->curr_wait_time += time_slice;
    return time_slice;
}

bool Process::update_wait_time(const int time_slice) {
    curr_wait_time += time_slice;
    if (curr_wait_time > max_wait_time) {
        this->curr_wait_time -= max_wait_time;
        return true;
    }
    return false;
}

std::string Process::detailed_print() const {
    std::stringstream ss;

    ss << "--------------------------\n";
    ss << "Process: " << name << '\n';
    ss << "Current wait time:\t" << curr_wait_time << '\n';
    ss << "Current execution time:\t" << curr_exec_time << '\n';
    ss << "Max wait time:\t" << max_wait_time << '\n';
    ss << "Time to completion:\t" << complete_time << '\n';
    ss << "--------------------------\n";

    return ss.str();
}

bool operator==(const Process &a, const Process &b) {
    return a.curr_wait_time == b.curr_wait_time;
}

std::ostream & operator<<(std::ostream &os, const Process &obj){
    if (&obj)
        os << obj.name << obj.curr_wait_time;
    else
        os << "";
    return os;
}
