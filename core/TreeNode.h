#ifndef RB_TREE_PROCESS_SCHEDULER_TREENODE_H
#define RB_TREE_PROCESS_SCHEDULER_TREENODE_H

#include "Process.h"
#include <ostream>
#include <unordered_set>

class TreeNode {

public:
    TreeNode();

    explicit TreeNode(Process *mid);

    TreeNode* get_root() const;

    // Find first process with given current waiting and execution time
    Process* find(int wait_time, int exec_time) const;

    // Find tree node where certain process is located
    TreeNode *find_node(const Process *process) const;

    // Insert process into RB Tree
    void insert(Process* process);

    // Remove process from RB Tree
    void remove(Process* process);

    // Remove first process with given current waiting and execution time
    void remove(int wait_time, int exec_time, std::string *msg);

    // Print tree as a 2-3-4 tree
    void print(std::ostream &os);

    // Print tree as a red-black tree
    void print_rb(std::ostream &os);

    // Simulate one step of process scheduling execution
    void simulation_step(int time_slice, std::ostream &os);

    // Simulate process scheduling execution until no processes are left in tree
    void simulate(int time_slice, std::ostream &os);

    // Return number of nodes in tree node
    int n_count() const;

private:
    Process *find_leftmost_process() const;

    // Update tree after simulation step
    Process *update_tree(int time_slice, std::unordered_set<Process *> &updated, std::ostream &os);

    // Helper function for removing from tree
    void fix_up(TreeNode* node);

    // Returns number of children tree nodes
    int children_count() const;

    bool is_leaf();

    bool is_full();

    // Helper functions for inserting process into tree based on current count

    int one_insert(Process *process, TreeNode *insert_node);

    int two_insert(Process *process, TreeNode *insert_node);

    void three_insert(Process* process, const TreeNode* insert_node);


    Process *find_successor(Process *process, int p_pos);

    // Check if tree nodes are 'real' brothers
    static bool are_real_brothers(TreeNode* a, TreeNode* b);

    friend std::ostream& operator<<(std::ostream& os, const TreeNode& obj);

    // Data members for association

    TreeNode* root;

    Process** data;

    TreeNode** p;

    TreeNode* parent;

    int node_count;

    friend class RedBlackTree;
};

inline TreeNode *TreeNode::get_root() const { return root; }


#endif //RB_TREE_PROCESS_SCHEDULER_TREENODE_H