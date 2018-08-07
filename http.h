#ifndef HTTP_H_FILE
#define HTTP_H_FILE

#include <string.h>
#include "list.h"

enum verb {
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

// MANDATORY to call !
void prepare_parser();
enum verb get_verb(char* uri);
char* get_url(char* uri);

#endif //HTTP_H_FILE
