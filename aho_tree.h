#ifndef AHO_TREE_H_FILE
#define AHO_TREE_H_FILE

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

// simple tree used to perform Aho-Corasick algorithm - note: this is unsuitable for matching long string, due to the overhead of pointer-chasing
struct aho_tree {
    char content;
    struct aho_tree *children;
    // As we match 'char', there cannot be more than 256 descendants per node
    uint8_t children_num;
    // this field is only useful for terminal points (aka nodes with no children)
    uint64_t data;
};

struct match {
    struct aho_tree *node;
    size_t length_matched;
};

struct aho_tree tree_new();
struct match tree_match(struct aho_tree *t, char *buf);
void tree_add(struct aho_tree *t, char* buf, uint64_t val);

#endif //AHO_TREE_H_FILE
