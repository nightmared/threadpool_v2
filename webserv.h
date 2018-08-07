#ifndef WEBSERV_H_FILE
#define WEBSERV_H_FILE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include "list.h"
#include "http.h"


struct http_query {
    enum verb verb;
    char *url;
    struct list_node *headers;
};

void http_parse(char *buf, size_t len, int (*cb)(struct http_query*));
void http_query_destroy(struct http_query *q);
void webserv(void* sock);

#endif //WEBSERV_H_FILE
