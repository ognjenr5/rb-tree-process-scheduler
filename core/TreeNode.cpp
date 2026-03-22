#include <stack>
#include <queue>

#include "TreeNode.h"
#include "RedBlackTree.h"
#include "Process.h"

TreeNode::TreeNode() : root(this), parent(nullptr) {
    // Initialize empty tree node
    this->data = new Process* [3];
    this->p = new TreeNode* [4];

    for (int i = 0; i < 3; i++)
        this->data[i] = nullptr;

    for (int i = 0; i < 4; i++)
        this->p[i] = nullptr;
}

TreeNode::TreeNode(Process *mid) : TreeNode() {
    // Create tree node with set node in the middle
    this->data[1] = mid;
    mid->set_node(this);
}

Process* TreeNode::find(const int wait_time, const int exec_time) const {
    int prev = -1;
    TreeNode* curr = this->root;

    // Loop until process is found or certainly doesn't exist
    while (true) {
        int pos = -1;

        // Loop over the 3 nodes
        for (int i = 0; i < 3; i++) {
            if (curr->data[i]) {
                prev = i;

                // Process is found, return it
                if (curr->data[i]->get_wait_time() == wait_time && curr->data[i]->get_exec_time() == exec_time)
                    return curr->data[i];

                // Process is located inside the subtree left of the current node
                if (Process::compare(wait_time, exec_time, curr->data[i])) {
                    pos = i;
                    break;
                }

            }
        }

        // Process is located in the subtree to the right of the last node
        if (prev != -1 && curr->data[prev] && pos == -1)
            pos = prev+1;

        // Process doesn't exist inside the tree
        if (prev == -1 || curr->is_leaf() || curr->p[pos] == nullptr)
            return nullptr;

        curr = curr->p[pos];
    }
}

TreeNode* TreeNode::find_node(const Process* process) const {

    int prev = -1;
    TreeNode* curr = this->root;
    TreeNode* insert_node = nullptr;

    // Loop until node containing the process is found
    while (true) {
        int pos = -1;
        insert_node = curr;

        // Loop over the 3 nodes
        for (int i = 0; i < 3; i++) {
            if (curr->data[i]) {
                prev = i;

                // Process is found
                if (Process::compare(process, curr->data[i])) {
                    pos = i;
                    break;
                }
            }
        }

        // Process is located in the subtree to the right of the last node
        if (prev != -1 && curr->data[prev] && pos == -1)
            pos = prev+1;

        // Process doesn't exist inside the tree
        if (prev == -1 || curr->is_leaf() || curr->p[pos] == nullptr)
            break;

        curr = curr->p[pos];
    }

    return insert_node;
}

int TreeNode::children_count() const {
    int cnt = 0;
    for (int i = 0; i < 4; i++)
        if (this->p[i]) cnt++;
    return cnt;
}

int TreeNode::n_count() const {
    int cnt = 0;
    for (int i = 0; i < 3; i++)
        if (this->data[i]) cnt ++;
    return cnt;
}

void TreeNode::insert(Process* process) {
    TreeNode* insert_node = this->find_node(process);

    // Insert into tree node based on number of nodes inside it
    switch (insert_node->n_count()) {
        case 1:
            one_insert(process, insert_node);
            break;
        case 2:
            two_insert(process, insert_node);
            break;
        case 3:
            three_insert(process, insert_node);
            break;

        // Empty tree node, only happens when inserting into an empty tree
        default:
            this->root = insert_node;
            this->root->data[1] = process;
            process->set_node(this->root);
    }

}

int TreeNode::one_insert(Process* process, TreeNode* insert_node) {

    // Left or right of the current node, which will always be in the middle
    const int pos = Process::compare(process, insert_node->data[1]) ? 0 : 2;

    // Insert process and set its parent
    insert_node->data[pos] = process;
    process->set_node(insert_node);

    // Return inserted position
    return pos;
}

int TreeNode::two_insert(Process* process, TreeNode* insert_node) {
    // Insert based on position relative to existing processes and current tree node layout

    int pos;

    // Node looks like: [-|X|X]
    if (insert_node->data[2]) {
        if (Process::compare(process, insert_node->data[1]))
            pos = 0;
        else if (Process::compare(insert_node->data[2], process) || *insert_node->data[2] == *process){
            insert_node->data[0] = insert_node->data[1];
            insert_node->data[1] = insert_node->data[2];
            pos = 2;

            insert_node->p[0] = insert_node->p[1];
            insert_node->p[1] = insert_node->p[2];
        }
        else {
            insert_node->data[0] = insert_node->data[1];
            pos = 1;

            insert_node->p[0] = insert_node->p[1];
        }
    }

    // Node looks like: [X|X|-]
    else {
        if (Process::compare(insert_node->data[1], process) || *insert_node->data[1] == *process)
            pos = 2;
        else if (Process::compare(process, insert_node->data[0])){
            insert_node->data[2] = insert_node->data[1];
            insert_node->data[1] = insert_node->data[0];
            pos = 0;

            insert_node->p[3] = insert_node->p[2];
            insert_node->p[2] = insert_node->p[1];
        }
        else {
            insert_node->data[2] = insert_node->data[1];
            pos = 1;
            insert_node->p[3] = insert_node->p[2];
        }
    }

    insert_node->data[pos] = process;
    process->set_node(insert_node);

    return pos;
}

void TreeNode::three_insert(Process *process, const TreeNode *insert_node) {
    // Insert into a full node

    std::stack<TreeNode*> stack;
    TreeNode* left = new TreeNode(insert_node->data[0]);
    TreeNode* right = new TreeNode(insert_node->data[2]);

    // Decide if process goes to the left or right of the middle process
    if (Process::compare(process, insert_node->data[1])) {
        // Decide position of the process in the split left node
        if (Process::compare(process, insert_node->data[0]))
            left->data[0] = process;
        else
            left->data[2] = process;
        process->set_node(left);
    }

    else {
        // Decide position of the process in the split right node
        if (Process::compare(process, insert_node->data[2]))
            right->data[0] = process;
        else
            right->data[2] = process;
        process->set_node(right);
    }

    // Propagate the split node up the tree
    Process* up = insert_node->data[1];
    TreeNode* f = insert_node->parent;

    // Loop until tree root is reached or the tree node isn't full
    while (f && f->n_count() == 3) {
        auto pl = left, pr = right;
        insert_node = f;
        f = insert_node->parent;
        left = new TreeNode(insert_node->data[0]);
        right = new TreeNode(insert_node->data[2]);

        // Decide if the node goes to the left or to the right of the middle node
        if (Process::compare(up, insert_node->data[1])) {
            right->p[1] = insert_node->p[2];
            if (right->p[1]) right->p[1]->parent = right;
            right->p[2] = insert_node->p[3];
            if (right->p[2]) right->p[2]->parent = right;

            pl->parent = left;
            pr->parent = left;
            up->set_node(left);
            if (Process::compare(up, insert_node->data[0])) {
                left->data[0] = up;
                left->p[0] = pl;
                left->p[1] = pr;
                left->p[2] = insert_node->p[1];
                if (left->p[2]) left->p[2]->parent = left;
            }

            else {
                left->data[2] = up;
                left->p[1] = insert_node->p[0];
                if (left->p[1]) left->p[1]->parent = left;
                left->p[2] = pl;
                left->p[3] = pr;
            }

        }
        else {
            left->p[1] = insert_node->p[0];
            if (left->p[1]) left->p[1]->parent = left;
            left->p[2] = insert_node->p[1];
            if (left->p[2]) left->p[2]->parent = left;
            pl->parent = right;
            pr->parent = right;
            up->set_node(right);
            if (Process::compare(up, insert_node->data[2])) {
                right->data[0] = up;
                right->p[0] = pl;
                right->p[1] = pr;
                right->p[2] = insert_node->p[3];
                if (right->p[2]) right->p[2]->parent = right;
            }
            else {
                right->data[2] = up;
                right->p[1] = insert_node->p[2];
                if (right->p[1]) right->p[1]->parent = right;
                right->p[2] = pl;
                right->p[3] = pr;
            }

        }
        up = insert_node->data[1];
    }

    // If the last node while going up exists, that means it's not full, insert accordingly
    if (f) {
        int insert_pos = 0;
        if (f->n_count() == 1)
            insert_pos = one_insert(up, f);
        else if (f->n_count() == 2)
            insert_pos = two_insert(up, f);

        f->p[insert_pos] = left;
        f->p[insert_pos + 1] = right;

        left->parent = f;
        right->parent = f;
    }

    // Otherwise, tree root splits into three nodes, and the middle one is the new root
    else {
        f = new TreeNode(up);
        f->p[1] = left;
        f->p[2] = right;
        this->root = f;

        left->parent = this->root;
        right->parent = this->root;
    }
}

Process* TreeNode::find_successor(Process* process, int p_pos) {
    // Start from the node to the right of the current process containing node
    TreeNode* curr = process->get_node()->p[p_pos + 1];

    // Loop until leaf is reached
    while (!curr->is_leaf()) {
        if (curr->p[0]) curr = curr->p[0];
        else if (curr->p[1]) curr = curr->p[1];
        else if (curr->p[2]) curr = curr->p[2];
        else if (curr->p[3]) curr = curr->p[3];
    }

    // Successor is the leftmost existing process in the tree
    Process* successor;

    if (curr->data[0])
        successor = curr->data[0];
    else if (curr->data[1])
        successor = curr->data[1];
    else
        successor = curr->data[2];

    return successor;
}

void TreeNode::fix_up(TreeNode *n) {
    TreeNode* node = n;
    TreeNode* prev = nullptr;
    do {
        // Loop until root is reached

        prev = node;
        node = node->parent;
        TreeNode* parent = node->parent;

        int prev_child_pos = node->p[0] == prev ? 0 : node->p[1] == prev ? 1 : node->p[2] == prev ? 2 : 3;
        int par_pos = parent->p[0] == node ? 0 : parent->p[1] == node ? 1 : parent->p[2] == node ? 2 : 3;

        TreeNode* right_brother = par_pos != 3 ? node->parent->p[par_pos+1] : nullptr;
        bool real_right = are_real_brothers(node, right_brother);
        TreeNode* left_brother = par_pos != 0 ? node->parent->p[par_pos-1] : nullptr;
        bool real_left = are_real_brothers(node, left_brother);

        // Exchange with real right brother
        if (real_right && right_brother->n_count() > 1) {

            if (right_brother->n_count() == 2) {
                int leftmost = right_brother->data[0] ? 0 : 1;
                node->data[1] = parent->data[par_pos];
                node->data[1]->set_node(node);
                parent->data[par_pos] = right_brother->data[leftmost];
                parent->data[par_pos]->set_node(parent);

                node->p[1] = node->p[prev_child_pos];
                node->p[0] = nullptr;

                if (leftmost == 0) {
                    right_brother->data[0] = nullptr;

                    node->p[2] = right_brother->p[0]; if (node->p[2]) node->p[2]->parent = node;
                    right_brother->p[0] = nullptr;
                }
                else {
                    right_brother->data[1] = right_brother->data[2];
                    right_brother->data[2] = nullptr;

                    node->p[2] = right_brother->p[1]; if (node->p[2]) node->p[2]->parent = node;
                    right_brother->p[1] = right_brother->p[2];
                    right_brother->p[2] = right_brother->p[3];
                    right_brother->p[3] = nullptr;
                }
            }
            else {
                node->data[1] = parent->data[par_pos];
                node->data[1]->set_node(node);
                parent->data[par_pos] = right_brother->data[1];
                parent->data[par_pos]->set_node(parent);
                node->data[2] = right_brother->data[0];
                node->data[2]->set_node(node);
                right_brother->data[1] = right_brother->data[2];
                right_brother->data[0] = nullptr;
                right_brother->data[2] = nullptr;

                // Pointer adjustment
                node->p[1] = node->p[prev_child_pos];
                node->p[2] = right_brother->p[0]; if (node->p[2]) node->p[2]->parent = node;
                node->p[3] = right_brother->p[1]; if (node->p[3]) node->p[3]->parent = node;
                node->p[0] = nullptr;
                right_brother->p[0] = nullptr;
                right_brother->p[1] = right_brother->p[2];
                right_brother->p[2] = right_brother->p[3];
                right_brother->p[3] = nullptr;
            }
        }

        // exchange with real left brother
        else if (real_left && left_brother->n_count() > 1) {
            if (left_brother->n_count() == 2) {
                int rightmost = left_brother->data[2] ? 2 : 1;
                node->data[1] = parent->data[par_pos-1];
                node->data[1]->set_node(node);
                parent->data[par_pos-1] = left_brother->data[rightmost];
                parent->data[par_pos-1]->set_node(parent);

                node->p[2] = node->p[prev_child_pos];
                node->p[3] = nullptr;

                if (rightmost == 2) {
                    left_brother->data[2] = nullptr;

                    node->p[1] = left_brother->p[3]; if (node->p[1]) node->p[1]->parent = node;
                    left_brother->p[3] = nullptr;
                }
                else {
                    left_brother->data[1] = left_brother->data[0];
                    left_brother->data[0] = nullptr;

                    node->p[1] = left_brother->p[2]; if (node->p[1]) node->p[1]->parent = node;
                    left_brother->p[2] = left_brother->p[1];
                    left_brother->p[1] = left_brother->p[0];
                    left_brother->p[0] = nullptr;
                }

            }
            else {
                node->data[1] = parent->data[par_pos-1];
                node->data[1]->set_node(node);
                parent->data[par_pos-1] = left_brother->data[1];
                parent->data[par_pos-1]->set_node(parent);
                node->data[0] = left_brother->data[2];
                node->data[0]->set_node(node);
                left_brother->data[1] = left_brother->data[0];
                left_brother->data[0] = nullptr;
                left_brother->data[2] = nullptr;

                // Pointer adjustment
                node->p[2] = node->p[prev_child_pos];
                node->p[0] = left_brother->p[2]; if (node->p[0]) node->p[0]->parent = node;
                node->p[1] = left_brother->p[3]; if (node->p[1]) node->p[1]->parent = node;
                node->p[3] = nullptr;
                left_brother->p[3] = nullptr;
                left_brother->p[2] = left_brother->p[1];
                left_brother->p[1] = left_brother->p[0];
                left_brother->p[0] = nullptr;
            }
        }

        // Join with real right brother
        else if (real_right) {
            right_brother->data[2] = right_brother->data[1];
            right_brother->data[1] = parent->data[par_pos];
            right_brother->data[1]->set_node(right_brother);
            parent->data[par_pos] = nullptr;
            if (par_pos == 0)
                parent->p[par_pos] = nullptr;
            else {
                parent->p[par_pos] = right_brother;
                parent->p[par_pos+1] = nullptr;
            }

            right_brother->p[3] = right_brother->p[2];
            right_brother->p[2] = right_brother->p[1];
            right_brother->p[1] = node->p[prev_child_pos];
            if (right_brother->p[1]) right_brother->p[1]->parent = right_brother;

            // Delete node;
            auto tmp = node;
            node = right_brother;
            node->parent = parent;
            delete tmp;
        }

        // Join with real left brother
        else if (real_left) {
            left_brother->data[0] = left_brother->data[1];
            left_brother->data[1] = parent->data[par_pos-1];
            left_brother->data[1]->set_node(left_brother);
            parent->data[par_pos-1] = nullptr;
            if (par_pos == 3)
                parent->p[par_pos] = nullptr;
            else {
                parent->p[par_pos] = left_brother;
                parent->p[par_pos-1] = nullptr;
            }

            left_brother->p[0] = left_brother->p[1];
            left_brother->p[1] = left_brother->p[2];
            left_brother->p[2] = node->p[prev_child_pos];
            if (left_brother->p[2]) left_brother->p[2]->parent = left_brother;

            // Delete node;
            auto tmp = node;
            node = left_brother;
            node->parent = parent;
            delete tmp;
        }

        // If there are no real brothers and if a right brother exists,
        // the parent node will look like [ |X|X], exchange with fake right brother

        else if (right_brother && right_brother->n_count() > 1) {
            node->data[1] = parent->data[1];
            node->data[1]->set_node(node);
            parent->data[1] = parent->data[2];
            parent->data[2] = nullptr;

            if (right_brother->n_count() == 2) {
                int leftmost = right_brother->data[0] ? 0 : 1;
                parent->data[0] = right_brother->data[leftmost];
                parent->data[0]->set_node(parent);

                node->p[1] = node->p[prev_child_pos];
                node->p[0] = nullptr;

                if (leftmost == 0) {
                    right_brother->data[0] = nullptr;

                    node->p[2] = right_brother->p[0]; if (node->p[2]) node->p[2]->parent = node;
                    right_brother->p[0] = nullptr;
                }
                else {
                    right_brother->data[1] = right_brother->data[2];
                    right_brother->data[2] = nullptr;

                    node->p[2] = right_brother->p[1]; if (node->p[2]) node->p[2]->parent = node;
                    right_brother->p[1] = right_brother->p[2];
                    right_brother->p[2] = right_brother->p[3];
                    right_brother->p[3] = nullptr;
                }
            }
            else {
                parent->data[0] = right_brother->data[1];
                parent->data[0]->set_node(parent);
                node->data[2] = right_brother->data[0];
                node->data[2]->set_node(node);
                right_brother->data[0] = nullptr;
                right_brother->data[1] = right_brother->data[2];
                right_brother->data[2] = nullptr;

                node->p[1] = node->p[prev_child_pos];
                node->p[2] = right_brother->p[0]; if (node->p[2]) node->p[2]->parent = node;
                node->p[3] = right_brother->p[1]; if (node->p[3]) node->p[3]->parent = node;
                node->p[0] = nullptr;
                right_brother->p[0] = nullptr;
                right_brother->p[1] = right_brother->p[2];
                right_brother->p[2] = right_brother->p[3];
                right_brother->p[3] = nullptr;
            }

            parent->p[0] = parent->p[1];
            parent->p[1] = parent->p[2];
            parent->p[2] = parent->p[3];
            parent->p[3] = nullptr;

        }

        // If there are no real brothers and if a right brother doesn't have enough to exchange,
        // the parent node will look like [X|X| ], exchange with fake left brother
        else if (left_brother && left_brother->n_count() > 1) {
            node->data[1] = parent->data[1];
            node->data[1]->set_node(node);
            parent->data[1] = parent->data[0];
            parent->data[0] = nullptr;

            node->p[2] = node->p[prev_child_pos];
            node->p[3] = nullptr;

            if (left_brother->n_count() == 2) {
                int rightmost = left_brother->data[2] ? 2 : 1;
                parent->data[2] = left_brother->data[rightmost];
                parent->data[2]->set_node(parent);
                if (rightmost == 2) {
                    left_brother->data[2] = nullptr;

                    node->p[1] = left_brother->p[3]; if (node->p[1]) node->p[1]->parent = node;
                    left_brother->p[3] = nullptr;
                }
                else {
                    left_brother->data[1] = left_brother->data[0];
                    left_brother->data[0] = nullptr;

                    node->p[1] = left_brother->p[2]; if (node->p[1]) node->p[1]->parent = node;
                    left_brother->p[2] = left_brother->p[1];
                    left_brother->p[1] = left_brother->p[0];
                    left_brother->p[0] = nullptr;
                }
            }
            else {
                parent->data[2] = left_brother->data[1];
                parent->data[2]->set_node(parent);
                node->data[0] = left_brother->data[0];
                node->data[0]->set_node(node);
                left_brother->data[1] = left_brother->data[0];
                left_brother->data[0] = nullptr;

                //pointer adjustment
                node->p[2] = node->p[prev_child_pos];
                node->p[0] = left_brother->p[2]; if (node->p[0]) node->p[0]->parent = node;
                node->p[1] = left_brother->p[3]; if (node->p[1]) node->p[1]->parent = node;
                node->p[3] = nullptr;
                left_brother->p[3] = nullptr;
                left_brother->p[2] = left_brother->p[1];
                left_brother->p[1] = left_brother->p[0];
                left_brother->p[0] = nullptr;
            }

            parent->p[3] = parent->p[2];
            parent->p[2] = parent->p[1];
            parent->p[1] = parent->p[0];
            parent->p[0] = nullptr;
        }

        // Join with fake right brother
        else if (right_brother) {
            right_brother->data[2] = right_brother->data[1];
            right_brother->data[1] = parent->data[1];
            right_brother->data[1]->set_node(right_brother);
            parent->data[par_pos] = nullptr;
            if (parent->data[0]) {
                parent->data[1] = parent->data[0];
                parent->data[0] = nullptr;
                parent->p[1] = parent->p[0];
                parent->p[0] = nullptr;
                parent->p[2] = right_brother;
                parent->p[3] = nullptr;
            }
            else {
                parent->data[1] = parent->data[2];
                parent->data[2] = nullptr;
                parent->p[2] = parent->p[3];
                parent->p[3] = nullptr;
                parent->p[1] = right_brother;
                parent->p[0] = nullptr;
            }

            right_brother->p[3] = right_brother->p[2];
            right_brother->p[2] = right_brother->p[1];
            right_brother->p[1] = node->p[prev_child_pos];
            if (right_brother->p[1]) right_brother->p[1]->parent = right_brother;

            //delete node;
            auto tmp = node;
            node = right_brother;
            node->parent = parent;
            delete tmp;
        }

        // Join with fake left brother
        else if (left_brother) {
            left_brother->data[0] = left_brother->data[1];
            left_brother->data[1] = parent->data[1];
            left_brother->data[1]->set_node(left_brother);
            parent->data[par_pos] = nullptr;
            if (parent->data[0]) {
                parent->data[1] = parent->data[0];
                parent->data[0] = nullptr;
                parent->p[1] = parent->p[0];
                parent->p[0] = nullptr;
                parent->p[2] = left_brother;
                parent->p[3] = nullptr;
            }
            else {
                parent->data[1] = parent->data[2];
                parent->data[2] = nullptr;
                parent->p[2] = parent->p[3];
                parent->p[3] = nullptr;
                parent->p[1] = left_brother;
                parent->p[0] = nullptr;
            }

            left_brother->p[0] = left_brother->p[1];
            left_brother->p[1] = left_brother->p[2];
            left_brother->p[2] = node->p[prev_child_pos];
            if (left_brother->p[2]) left_brother->p[2]->parent = left_brother;

            // Delete node;
            auto tmp = node;
            node = left_brother;
            node->parent = parent;
            delete tmp;
        }
    } while (node->parent->parent && node->parent->n_count() == 0);

    if (node->parent->n_count() == 0 && !node->parent->parent) {
        this->root = node;
        delete node->parent;
        this->root->parent = nullptr;
    }
}

void TreeNode::remove(Process *process) {
    TreeNode* node = process->get_node();
    int p_pos = node->data[0] == process ? 0 : node->data[1] == process ? 1 : 2;

    if (!node->is_leaf()) {
        Process* succ = this->find_successor(process, p_pos);
        TreeNode* succ_node = succ->get_node();
        int s_pos = succ_node->data[0] == succ ? 0 : succ_node->data[1] == succ ? 1 : 2;

        node->data[p_pos] = succ;
        succ->set_node(node);

        // Continue as if removing successor from its node
        node = succ_node;
        p_pos = s_pos;
    }

    // Removing from [X|X|X] or [X|X| ] or [ |X|X]
    if (node->n_count() > 1) {
        if (p_pos == 0 || p_pos == 2) {
            node->data[p_pos] = nullptr;
        }
        else {
            if (node->data[2]) {
                node->data[1] = node->data[2];
                node->data[2] = nullptr;
            }
            else {
                node->data[1] = node->data[0];
                node->data[0] = nullptr;
            }
        }
    }

    // Removing from [ |X| ]
    else {
        TreeNode* parent = node->parent;
        if (!parent) {
            node->data[1] = nullptr;
            node->node_count = 0;
            return;
        }
        int par_pos = parent->p[0] == node ? 0 : parent->p[1] == node ? 1 : parent->p[2] == node ? 2 : 3;

        TreeNode* right_brother = par_pos != 3 ? node->parent->p[par_pos+1] : nullptr;
        bool real_right = are_real_brothers(node, right_brother);
        TreeNode* left_brother = par_pos != 0 ? node->parent->p[par_pos-1] : nullptr;
        bool real_left = are_real_brothers(node, left_brother);

        // exchange with real right brother
        if (real_right && right_brother->n_count() > 1) {
            if (right_brother->n_count() == 2) {
                int leftmost = right_brother->data[0] ? 0 : 1;
                node->data[1] = parent->data[par_pos];
                node->data[1]->set_node(node);
                parent->data[par_pos] = right_brother->data[leftmost];
                parent->data[par_pos]->set_node(parent);

                if (leftmost == 0) right_brother->data[0] = nullptr;
                else {
                    right_brother->data[1] = right_brother->data[2];
                    right_brother->data[2] = nullptr;
                }
            }
            else {
                node->data[1] = parent->data[par_pos];
                node->data[1]->set_node(node);
                parent->data[par_pos] = right_brother->data[1];
                parent->data[par_pos]->set_node(parent);
                node->data[2] = right_brother->data[0];
                node->data[2]->set_node(node);
                right_brother->data[1] = right_brother->data[2];
                right_brother->data[0] = nullptr;
                right_brother->data[2] = nullptr;
            }
        }

        // Exchange with real left brother
        else if (real_left && left_brother->n_count() > 1) {
            if (left_brother->n_count() == 2) {
                int rightmost = left_brother->data[2] ? 2 : 1;
                node->data[1] = parent->data[par_pos-1];
                node->data[1]->set_node(node);
                parent->data[par_pos-1] = left_brother->data[rightmost];
                parent->data[par_pos-1]->set_node(parent);

                if (rightmost == 2) left_brother->data[2] = nullptr;
                else {
                    left_brother->data[1] = left_brother->data[0];
                    left_brother->data[0] = nullptr;
                }
            }
            else {
                node->data[1] = parent->data[par_pos-1];
                node->data[1]->set_node(node);
                parent->data[par_pos-1] = left_brother->data[1];
                parent->data[par_pos-1]->set_node(parent);
                node->data[0] = left_brother->data[2];
                node->data[0]->set_node(node);
                left_brother->data[1] = left_brother->data[0];
                left_brother->data[0] = nullptr;
                left_brother->data[2] = nullptr;

            }
        }

        // Join with real right brother
        else if (real_right) {
            right_brother->data[2] = right_brother->data[1];
            right_brother->data[1] = parent->data[par_pos];
            right_brother->data[1]->set_node(right_brother);
            parent->data[par_pos] = nullptr;
            if (par_pos == 0) parent->p[par_pos] = nullptr;
            else {
                parent->p[par_pos] = right_brother;
                parent->p[par_pos+1] = nullptr;
            }
            if (parent->n_count() == 0) {
                if (parent->parent)
                    fix_up(right_brother);
                else {
                    this->root = right_brother;
                    delete right_brother->parent;
                    this->root->parent = nullptr;
                }
            }
        }

        // Join with real left brother
        else if (real_left) {
            left_brother->data[0] = left_brother->data[1];
            left_brother->data[1] = parent->data[par_pos-1];
            left_brother->data[1]->set_node(left_brother);
            parent->data[par_pos-1] = nullptr;
            if (par_pos == 3) parent->p[par_pos] = nullptr;
            else {
                parent->p[par_pos] = left_brother;
                parent->p[par_pos-1] = nullptr;
            }

            if (parent->n_count() == 0) {
                if (parent->parent)
                    fix_up(left_brother);
                else {
                    this->root = left_brother;
                    //delete left_brother->parent;
                    this->root->parent = nullptr;
                }
            }
        }

        // If there are no real brothers and if a right brother exists,
        // the parent node will look like [ |X|X], exchange with fake right brother

        else if (right_brother && right_brother->n_count() > 1) {
            node->data[1] = parent->data[1];
            node->data[1]->set_node(node);
            parent->data[1] = parent->data[2];
            parent->data[2] = nullptr;

            if (right_brother->n_count() == 2) {
                int leftmost = right_brother->data[0] ? 0 : 1;
                parent->data[0] = right_brother->data[leftmost];
                parent->data[0]->set_node(parent);
                if (leftmost == 0) right_brother->data[0] = nullptr;
                else {
                    right_brother->data[1] = right_brother->data[2];
                    right_brother->data[2] = nullptr;
                }
            }
            else {
                parent->data[0] = right_brother->data[1];
                parent->data[0]->set_node(parent);
                node->data[2] = right_brother->data[0];
                node->data[2]->set_node(node);
                right_brother->data[0] = nullptr;
                right_brother->data[1] = right_brother->data[2];
                right_brother->data[2] = nullptr;
            }

            parent->p[0] = parent->p[1];
            parent->p[1] = parent->p[2];
            parent->p[2] = parent->p[3];
            parent->p[3] = nullptr;
        }

        // If there are no real brothers and if a right brother doesn't have enough to exchange,
        // the parent node will look like [X|X| ], exchange with fake left brother
        else if (left_brother && left_brother->n_count() > 1) {
            node->data[1] = parent->data[1];
            node->data[1]->set_node(node);
            parent->data[1] = parent->data[0];
            parent->data[0] = nullptr;

            if (left_brother->n_count() == 2) {
                int rightmost = left_brother->data[2] ? 2 : 1;
                parent->data[2] = left_brother->data[rightmost];
                parent->data[2]->set_node(parent);
                if (rightmost == 2) left_brother->data[2] = nullptr;
                else {
                    left_brother->data[1] = left_brother->data[0];
                    left_brother->data[0] = nullptr;
                }
            }
            else {
                parent->data[2] = left_brother->data[1];
                parent->data[2]->set_node(parent);
                node->data[0] = left_brother->data[0];
                node->data[0]->set_node(node);
                left_brother->data[1] = left_brother->data[0];
                left_brother->data[0] = nullptr;
            }

            parent->p[3] = parent->p[2];
            parent->p[2] = parent->p[1];
            parent->p[1] = parent->p[0];
            parent->p[0] = nullptr;
        }

        // Join with fake right brother
        else if (right_brother) {
            right_brother->data[2] = right_brother->data[1];
            right_brother->data[1] = parent->data[1];
            right_brother->data[1]->set_node(right_brother);
            parent->data[par_pos] = nullptr;
            if (parent->data[0]) {
                parent->data[1] = parent->data[0];
                parent->data[0] = nullptr;
                parent->p[1] = parent->p[0];
                parent->p[0] = nullptr;
                parent->p[2] = right_brother;
                parent->p[3] = nullptr;
            }
            else {
                parent->data[1] = parent->data[2];
                parent->data[2] = nullptr;
                parent->p[2] = parent->p[3];
                parent->p[3] = nullptr;
                parent->p[1] = right_brother;
                parent->p[0] = nullptr;
            }

            if (parent->n_count() == 0) {
                if (parent->parent)
                    fix_up(right_brother);
                else {
                    this->root = right_brother;
                    delete right_brother->parent;
                    this->root->parent = nullptr;
                }
            }
        }

        // Join with fake left brother
        else if (left_brother) {
            left_brother->data[0] = left_brother->data[1];
            left_brother->data[1] = parent->data[1];
            left_brother->data[1]->set_node(left_brother);
            parent->data[par_pos] = nullptr;
            if (parent->data[0]) {
                parent->data[1] = parent->data[0];
                parent->data[0] = nullptr;
                parent->p[1] = parent->p[0];
                parent->p[0] = nullptr;
                parent->p[2] = left_brother;
                parent->p[3] = nullptr;
            }
            else {
                parent->data[1] = parent->data[2];
                parent->data[2] = nullptr;
                parent->p[2] = parent->p[3];
                parent->p[3] = nullptr;
                parent->p[1] = left_brother;
                parent->p[0] = nullptr;
            }

            if (parent->n_count() == 0) {
                if (parent->parent)
                    fix_up(left_brother);
                else {
                    this->root = left_brother;
                    delete left_brother->parent;
                    this->root->parent = nullptr;
                }
            }
        }
    }
}

void TreeNode::remove(const int wait_time, const int exec_time, std::string * msg) {
    // Remove handler function
    if (auto process = this->root->find(wait_time, exec_time)) {
        this->remove(process);

        *msg = "Process successfully deleted:\n" + process->detailed_print();
    }
    else *msg = "Process not found. Deletion failed.\n";
}

void TreeNode::print_rb(std::ostream& os) {

    // If the original tree is empty, print a message and stop
    if (this->root->n_count() == 0) {
        os << "\t\t\tTree is empty" << '\n';
        return;
    }

    // Convert the current 2-3-4 tree into an equivalent Red-Black tree
    RedBlackTree*  next = RedBlackTree::build_tree(this);

    std::stack<RedBlackTree*> s;
    std::stack<unsigned> level;
    unsigned lvl = 0;

    // Iterative postorder traversal of the RB Tree, having the changed order right -> parent -> left
    while (true) {
        while (next != nullptr) {
            s.push(next);
            level.push(lvl++);
            next = next->right;
        }
        if (!s.empty()) {
            next = s.top(); s.pop();
            lvl = level.top(); level.pop();

            for (int i = 0; i < lvl; i++) os << "\t";

            // Black nodes are printed in parentheses
            if (next->color == RedBlackTree::BLACK)
                os << "(" << *next->p << ")\n";
            else os << " " << *next->p << " \n";

            next = next->left;
            lvl++;
        }
        else break;
    }
    os << "\n------------------------------------------------------------------------------------------------------------------------\n";
}

void TreeNode::print(std::ostream& os) {
    // Queue used for level-order (BFS) traversal of the tree
    std::queue<TreeNode*> q;

    os << "\n";

    // Determine printing width depending on the output stream
    int width = os.rdbuf() == std::cout.rdbuf() ? 120 : 250;
    int space_count = (width-12) / 2, child_count = 0;

    // Print initial indentation
    for (int i = 0; i < space_count; i ++) os << " ";

    // If the tree is empty, print a message and exit
    if (!this->root->n_count()) {
         os << "Tree is empty.\n";
        return;
    }

    q.emplace(this->root);
    q.emplace(nullptr);


    // Level-order traversal of the tree
    while (!q.empty()) {
        TreeNode* curr = q.front();
        q.pop();

        // If node isn't a separator, print it and add children to queue
        if (curr) {
            os << *curr;
            for (int i = 0; i < 4; i ++) {
                if (curr->p[i]) q.emplace(curr->p[i]);
            }
            for (int i = 0; i < space_count; i ++) os << " ";
            child_count += curr->children_count();
        }
        // Otherwise, end of current level is detected, change indentation
        else {
            os << "\n\n";
            space_count = std::max(1, (width - 12 * child_count) / (child_count+1));
            for (int i = 0; i < space_count; i ++)
                os << " ";
            if (!q.empty()) q.push(nullptr);
            child_count = 0;
        }
    }
    os << "\n------------------------------------------------------------------------------------------------------------------------\n";
}

bool TreeNode::is_leaf() {
    for (int i = 0; i < 4; i++)
        if (this->p[i]) return false;
    return true;
}

bool TreeNode::is_full() {
    return this->data[0] && this->data[1] && this->data[2];
}

bool TreeNode::are_real_brothers(TreeNode *a, TreeNode *b) {
    if (!a || !b) return false;
    if (!a->parent || !b->parent || a->parent != b->parent) return false;

    TreeNode* parent = a->parent;
    if (parent->n_count() == 1 || parent->n_count() == 0) return true;

    if (parent->n_count() == 2) {

        // parent: [X|X| ]
        if (parent->data[0] && parent->data[1])
            return (a == parent->p[0] && b == parent->p[1]) || (a == parent->p[1] && b == parent->p[0]);

        // parent: [ |X|X]
        return (a == parent->p[2] && b == parent->p[3]) || (a == parent->p[3] && b == parent->p[2]);
    }

    // parent: [X|X|X]
    // Real brothers are first two and last two
    return  (a == parent->p[0] && b == parent->p[1]) || (a == parent->p[1] && b == parent->p[0]) ||
            (a == parent->p[2] && b == parent->p[3]) || (a == parent->p[3] && b == parent->p[2]);

}

Process* TreeNode::find_leftmost_process() const {
    TreeNode* curr = this->root;

    // Loop until leftmost tree node is found
    while (true) {
        int pos = -1;

        // Find the first existing child (leftmost child pointer)
        for (int i = 0; i < 4; i ++) {
            if (curr->p[i]) {
                pos = i;
                break;
            }
        }

        // If no children exist, this is a leaf node
        if (pos == -1) {
            for (int i = 0; i < 3; i ++) {
                if (curr->data[i]) return curr->data[i];
            }
        }
        // Otherwise, move to the leftmost child and continue searching
        else curr = curr->p[pos];
    }
}

Process *TreeNode::update_tree(int time_slice, std::unordered_set<Process*> &updated, std::ostream &os) {
    TreeNode* next = this->root;
    std::stack<std::pair<TreeNode*, int>> s;

    std::queue<Process* > to_reset;
    while (true) {
        while (next) {
            s.push(std::make_pair(next, 0));
            next = next->p[0];
        }

        if (!s.empty()) {
            next = s.top().first;
            int prev = s.top().second;
            s.pop();

            if (prev < 3) {
                if (next->data[prev] && !updated.contains(next->data[prev])) {
                    updated.insert(next->data[prev]);

                    if (next->data[prev]->update_wait_time(time_slice)) {
                        os << "\nProcess: " << *next->data[prev] << " - wait time exceeded.";
                        return next->data[prev];
                    }
                }

                s.push(std::make_pair(next, prev + 1));
                next = next->p[prev+1];
            }

            else next = nullptr;
        }
        else break;
    }

    return nullptr;

}

void TreeNode::simulation_step(int time_slice, std::ostream& os) {
    Process* leftmost = this->find_leftmost_process();
    this->remove(leftmost);
    int exec_time = leftmost->execute_process(time_slice);
    os  << "Process: " << *leftmost << " - was executing for " << exec_time << " time units.\n";

    std::queue<Process* > to_remove;
    std::unordered_set<Process* > updated;

    while (true) {
        Process* p = this->update_tree(exec_time, updated, os);
        if (!p) break;
        this->remove(p);
        to_remove.push(p);
    }

    while (!to_remove.empty()) {
        Process* p = to_remove.front();
        to_remove.pop();
        this->insert(p);
    }


    if (exec_time == time_slice) {
        this->insert(leftmost);
        os << "\nProcess " << *leftmost << " - isn't finished.\n";
    }

    else os << "\nProcess " << *leftmost << " - is finished.\n";

    this->print(os);

}

void TreeNode::simulate(int time_slice, std::ostream& os) {
    int cnt = 0;
    while (this->root->n_count()) {
        os << ++cnt << ". step\n";
        simulation_step(time_slice, os);

    }
}

std::ostream & operator<<(std::ostream &os, const TreeNode &obj) {
    os << "[" << *obj.data[0] << "|(" << *obj.data[1] << ")|" << *obj.data[2]  << "]";
    return os;
}