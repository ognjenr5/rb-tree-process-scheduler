#ifndef RB_TREE_PROCESS_SCHEDULER_PROCESS_H
#define RB_TREE_PROCESS_SCHEDULER_PROCESS_H

#include <iostream>
#include <string>
#include <ostream>

class TreeNode;

class Process {
public:

    // Process class, info about name, time needed to complete process,
    // maximum wait time and current waiting and execution time
    Process(const std::string& name, int complete_time, int max_wait_time);

    [[nodiscard]] int get_wait_time() const;

    void set_wait_time(int wait_time);

    [[nodiscard]] int get_exec_time() const;

    void set_exec_time(int exec_time);

    [[nodiscard]] std::string get_name() const;

    [[nodiscard]] TreeNode* get_node() const;

    void set_node(TreeNode* node);

    // Execute process for a certain time slice
    int execute_process(int time_slice);

    // Update current waiting time for time slice
    bool update_wait_time(int time_slice);

    [[nodiscard]] std::string detailed_print() const;

    static bool compare(const Process* a, const Process* b);

    static bool compare(int wt, int et, const Process* p);

    friend bool operator==(const Process& a, const Process& b);

    friend std::ostream& operator<<(std::ostream& os, const Process& obj);

private:
    std::string name;

    int complete_time;

    int max_wait_time;

    int curr_wait_time;

    int curr_exec_time;

    TreeNode* containing_node;
};

inline int Process::get_wait_time() const { return curr_wait_time; }

inline void Process::set_wait_time(const int wait_time) { this->curr_wait_time = wait_time; }

inline int Process::get_exec_time() const { return this->curr_exec_time; }

inline void Process::set_exec_time(const int exec_time) { this->curr_exec_time = exec_time; }

inline std::string Process::get_name() const { return this->name; }

inline TreeNode* Process::get_node() const { return this->containing_node; }

inline void Process::set_node(TreeNode* node) { this->containing_node = node; }

inline bool Process::compare(const Process *a, const Process *b) {
    return a->curr_wait_time > b->curr_wait_time;
}

inline bool Process::compare(int wt, int et, const Process *p) {
    return wt > p->curr_wait_time;
}

#endif //RB_TREE_PROCESS_SCHEDULER_PROCESS_H