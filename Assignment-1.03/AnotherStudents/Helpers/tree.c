#include <stdlib.h>

#include "tree.h"
#include "helpers.h"

// See tree.h
Binary_Tree_T *new_binary_tree(void *data) {
    Binary_Tree_T *t;
    t = safe_malloc(sizeof(Binary_Tree_T));
    t->data = data;
    t->left = NULL;
    t->right = NULL;
    return t;
}

int count_leaf_nodes(Binary_Tree_T *t) { // NOLINT(misc-no-recursion)
    if (t->left == NULL && t->right == NULL) {
        return 1;
    } else {
        return count_leaf_nodes(t->left) + count_leaf_nodes(t->right);
    }
}

// See tree.h
void cleanup_binary_tree(Binary_Tree_T *t) { // NOLINT(misc-no-recursion)
    if (t->left != NULL) {
        cleanup_binary_tree(t->left);
    }
    if (t->right != NULL) {
        cleanup_binary_tree(t->right);
    }
    free(t->data);
    free(t);
}


