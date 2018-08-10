 #include "aho_tree.h"

struct aho_tree tree_new() {
    struct aho_tree t;
    t.content = '\0';
    t.children = NULL;
    t.children_num = 0;
    t.data = 0;
    return t;
}

// insert the buffer in the tree, with the certainty that there is no conflict possible (aka. no part of the buffer is shared with other entries in the tree)
void tree_insert_children_unconditionally(struct aho_tree *t, char* buf, uint64_t val) {
    assert(buf[0] != '\0');

    struct aho_tree *current_tree = t;
    size_t pos = 0;
    while (buf[pos] != '\0') {
        current_tree->children_num += 1;
        if (current_tree->children_num == 1) {
            // this node had no children previously, so we allocate one
            current_tree->children = malloc(sizeof(struct aho_tree));
        } else {
            current_tree->children = realloc(current_tree->children, current_tree->children_num*sizeof(struct aho_tree));
        }
        if (current_tree->children == NULL)
            exit(1);

        current_tree = &current_tree->children[current_tree->children_num-1];
        current_tree->children_num = 0;
        current_tree->content = buf[pos];

        pos++;
    }
    current_tree->data = val;
}

struct match tree_match(struct aho_tree *t, char *buf) {
    struct match m;
    struct aho_tree *current_tree = t;
    size_t pos = 0;
    uint8_t found;
    while (buf[pos] != '\0' && current_tree->children_num > 0) {
        found = 0;
        for (uint8_t child = 0; child < current_tree->children_num; child++) {
            if (current_tree->children[child].content == buf[pos]) {
                pos += 1;
                current_tree = &current_tree->children[child];
                found = 1;
                break;
            }
        }
        // No children allow us to purse...
        if (!found)
            break;
    }
    m.length_matched = pos;
    m.node = current_tree;
    return m;
}

void tree_add(struct aho_tree *t, char* buf, uint64_t val) {
    struct match common_part = tree_match(t, buf);
    // if the element to match is already in the tree, we just update its value
    if (common_part.length_matched == strlen(buf)) {
        common_part.node->data = val;
        return;
    }
    tree_insert_children_unconditionally(common_part.node, &buf[common_part.length_matched], val);
}

void tree_free(struct aho_tree *t) {
    if (t->children_num == 0)
        return;

    for (uint8_t child = 0; child < t->children_num; child++) {
        tree_free(&t->children[child]);
    }

    free(t->children);
    t->children_num = 0;
}
