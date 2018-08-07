#include "http.h"
#include "aho_tree.h"

#define MIN(a, b) (a > b ? b : a)
#define COMPARE_RETURN(str, val, strval) if (strncmp(str, strval, strlen(strval)) == 0) \
return val;

static struct aho_tree verb_match_tree;

void prepare_parser() {
    verb_match_tree = tree_new();
    tree_add(&verb_match_tree, "GET", GET);
    tree_add(&verb_match_tree, "POST", POST);
    tree_add(&verb_match_tree, "PUT", PUT);
    tree_add(&verb_match_tree, "HEAD", HEAD);
    tree_add(&verb_match_tree, "DELETE", DELETE);
    tree_add(&verb_match_tree, "OPTIONS", OPTIONS);
    tree_add(&verb_match_tree, "TRACE", TRACE);
    tree_add(&verb_match_tree, "CONNECT", CONNECT);
}

enum verb get_verb(char* uri) {
    struct match res = tree_match(&verb_match_tree, uri);
    return res.node->data;
}

int contain(char* src, char* cmp, size_t pos) {
    size_t srclen = strlen(src);
    size_t cmplen = strlen(cmp);

    if (cmplen+pos > srclen)
        return 0;

    size_t counter = 0;
    size_t maxlen = MIN(cmplen, srclen-pos);
    while (counter < maxlen && src[pos+counter] == cmp[counter])
        counter++;

    if (counter == cmplen)
        return 1;
    return 0;
}

char* get_url(char* uri) {
    size_t len = strlen(uri);
    size_t pos = 0;
    // ignore any CLRF before the Request-Line, per the specification (https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
    while (pos < len && (uri[pos] == '\n' || uri[pos] == '\r'))
        pos++;
    // ignore the HTTP verb
    while (pos < len && uri[pos] != ' ')
        pos++;
    // ignore the space after the HTTP verb
    pos++;

    if (contain(uri, " HTTP/1.", len-9)) {
        size_t copy_len = len-pos-8;
        char* res = malloc(copy_len);
        if (!res)
            ERR("malloc failed")
        strncpy(res, &uri[pos], copy_len-1);
        res[copy_len-1] = 0;
        return res;
    } else {
        return NULL;
    }
}


