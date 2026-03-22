#ifndef RB_TREE_PROCESS_SCHEDULER_REDBLACKTREE_H
#define RB_TREE_PROCESS_SCHEDULER_REDBLACKTREE_H

#include "Process.h"
#include "TreeNode.h"

class RedBlackTree {
public:
    explicit RedBlackTree(Process* p);

    // Create a Red-Black tree based on the equivalent 2-3-4 tree
    static RedBlackTree* build_tree(TreeNode *tree);

    enum Color {RED, BLACK};

private:
    RedBlackTree* left;

    RedBlackTree* right;

    Process* p;

    Color color;

    friend void TreeNode::print_rb(std::ostream& os);
};


#endif //RB_TREE_PROCESS_SCHEDULER_REDBLACKTREE_H