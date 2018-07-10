#ifndef WEBSERV_H_FILE
#define WEBSERV_H_FILE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include "list.h"

enum verb {
    GET,
    POST,
    PUT,
    HEAD,
    DELETE,
    INVALID
};


struct http_query {
    enum verb verb;
    char *url;
    struct list_node *headers;
};

enum verb get_verb(char* uri);
char* get_url(char* uri);
void http_parse(char *buf, size_t len, int (*cb)(struct http_query*));
void http_query_destroy(struct http_query *q);
void webserv(void* sock);

#endif //WEBSERV_H_FILE
