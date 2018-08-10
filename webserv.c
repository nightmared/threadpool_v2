#include "webserv.h"

int on_parsed(struct http_query* q) {
    // extract method & URL
    printf("%s\n", q->url);
    return 0;
}

void webserv(void* s) {
    int sock = (int)(long)s;

    size_t len = 80*1024;
    char buf[len];
    ssize_t recved = recv(sock, buf, len, 0);
    if (recved < 0) {
        // receive failed, let's forget this connection
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }

    http_parse(buf, recved, on_parsed);

    close(sock);
}
