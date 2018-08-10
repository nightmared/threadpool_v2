#ifndef WEBSERV_H_FILE
#define WEBSERV_H_FILE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include "list.h"
#include "http.h"

void webserv(void* sock);

#endif //WEBSERV_H_FILE
