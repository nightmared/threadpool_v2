#ifndef HTTP_H_FILE
#define HTTP_H_FILE

#include <string.h>
#include "list.h"

enum verb {
    // The INVALID state MUST have the value 0 to match the value returned by default by tree_match
    INVALID = 0,
    GET,
    POST,
    PUT,
    HEAD,
    DELETE,
    OPTIONS,
    TRACE,
    CONNECT
};

struct http_query {
    enum verb verb;
    char *url;
    char *body;
    struct list headers;
};

// MANDATORY to call !
void parser_prepare();
void http_parse(char *buf, size_t len, int (*cb)(struct http_query*));
void http_query_destroy(struct http_query *q);
void parser_cleanup();

#endif //HTTP_H_FILE
