#include "webserv.h"

int on_parsed(struct http_query* q) {
    // extract method & URL
    printf("%s\n", q->url);
    printf("%s\n", q->body);
    return 0;
}

void webserv(void* s) {
    int sock = (int)(long)s;

    ssize_t recvd;
    size_t len = 2048;
    char *buf = malloc(len);
    if (!buf)
        ERR("malloc failed")
    size_t count = 0;
    // add to the buffer while we can read
    while ((recvd = recv(sock, buf+count*len, len, 0)) == len) {
        count++;
        buf = realloc(buf, (count+1)*len);
        if (!buf)
            ERR("realloc failed")
    }
    if (recvd < 0) {
        // receive failed, let's forget this connection
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
    }

    http_parse(buf, count*len+recvd, on_parsed);
    free(buf);

    close(sock);
}
