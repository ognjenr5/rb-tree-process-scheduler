#include "RedBlackTree.h"

#include <queue>

RedBlackTree::RedBlackTree(Process* p) : left(nullptr), right(nullptr), p(p), color(BLACK) {}

RedBlackTree * RedBlackTree::build_tree(TreeNode* tree) {

    // Create the root of the Red-Black tree using the middle key of the 2-3-4 root
    RedBlackTree* rb_tree = new RedBlackTree(tree->get_root()->data[1]);
    rb_tree->p = tree->get_root()->data[1];

    // Queue of pointers to RB-tree child positions that need to be filled
    std::queue<RedBlackTree**> rb;

    // Handle the root depending on how many keys it contains
    if (tree->get_root()->n_count() == 1) {
        rb.push(&rb_tree->left);
        rb.push(&rb_tree->right);
    }

    else if (tree->get_root()->n_count() == 2) {
        if (tree->get_root()->data[0]) {
            rb_tree->left = new RedBlackTree(tree->get_root()->data[0]);
            rb_tree->left->color = RED;
            rb.push(&rb_tree->left->left);
            rb.push(&rb_tree->left->right);
            rb.push(&rb_tree->right);
        }
        else {
            rb_tree->right = new RedBlackTree(tree->get_root()->data[2]);
            rb_tree->right->color = RED;
            rb.push(&rb_tree->left);
            rb.push(&rb_tree->right->left);
            rb.push(&rb_tree->right->right);
        }
    }

    else {
        rb_tree->left = new RedBlackTree(tree->get_root()->data[0]);
        rb_tree->left->color = RED;
        rb_tree->right = new RedBlackTree(tree->get_root()->data[2]);
        rb_tree->right->color = RED;
        rb.push(&rb_tree->left->left);
        rb.push(&rb_tree->left->right);
        rb.push(&rb_tree->right->left);
        rb.push(&rb_tree->right->right);
    }

    // Add root children to traversal queue
    std::queue<TreeNode*> q;
    for (int i = 0; i < 4; i++) {
        if (tree->get_root()->p[i]) q.push(tree->get_root()->p[i]);
    }

    // BFS traversal of the 2-3-4 tree
    while (!q.empty()) {
        TreeNode* curr = q.front();
        q.pop();

        // Add children of the current node to BFS queue
        for (int i = 0; i < 4; i++) {
            if (curr->p[i]) q.push(curr->p[i]);
        }

        // Take the next RB-tree position that should be filled
        RedBlackTree** pointer = rb.front();
        rb.pop();

        // Convert the current 2-3-4 node to an equivalent RB-tree structure
        if (curr->n_count() == 1) {

            auto new_root = new RedBlackTree(curr->data[1]);
            *pointer = new_root;
            rb.push(&new_root->left);
            rb.push(&new_root->right);
        }

        else if (curr->n_count() == 2) {
            auto new_root = new RedBlackTree(curr->data[1]);

            if (curr->data[0]) {
                new_root->left = new RedBlackTree(curr->data[0]);
                new_root->left->color = RED;
                *pointer = new_root;

                rb.push(&new_root->left->left);
                rb.push(&new_root->left->right);
                rb.push(&new_root->right);
            }

            else {
                new_root->right = new RedBlackTree(curr->data[2]);
                new_root->right->color = RED;
                *pointer = new_root;

                rb.push(&new_root->left);
                rb.push(&new_root->right->left);
                rb.push(&new_root->right->right);
            }
        }

        else {
            auto new_root = new RedBlackTree(curr->data[1]);
            new_root->left = new RedBlackTree(curr->data[0]);
            new_root->left->color = RED;
            new_root->right = new RedBlackTree(curr->data[2]);
            new_root->right->color = RED;

            *pointer = new_root;

            rb.push(&new_root->left->left);
            rb.push(&new_root->left->right);
            rb.push(&new_root->right->left);
            rb.push(&new_root->right->right);

        }
    }

    return rb_tree;
}
