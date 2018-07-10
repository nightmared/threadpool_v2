#include "webserv.h"

void webserv(void* s) {
    int sock = (int)(long)s;
    close(sock);
}
