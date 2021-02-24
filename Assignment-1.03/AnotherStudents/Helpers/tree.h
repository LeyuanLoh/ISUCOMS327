#ifndef ROGUE_TREE_H
#define ROGUE_TREE_H

// We have to forward declare it otherwise we need annoying typecasts in the following struct.
typedef struct Binary_Tree_S Binary_Tree_T;

// Contains a binary tree that points to its children and the data. By using a void pointer, the tree can store any
// data, but it must be casted and the programmer is responsible for keeping track of what type of data is stored.
struct Binary_Tree_S {
    void *data;
    Binary_Tree_T *left;
    Binary_Tree_T *right;
};

// Returns a pointer to a new tree with no children and the pointer initialized to the data value.
Binary_Tree_T *new_binary_tree(void *data);

// Counts the number of leaf nodes (nodes with no children)
int count_leaf_nodes(Binary_Tree_T *t);

// Recursively visits all children, free()'ing the memory, and the memory pointed to in data. Then frees the root.
void cleanup_binary_tree(Binary_Tree_T *t);

#endif //ROGUE_TREE_H
