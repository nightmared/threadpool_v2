#include "webserv.h"

int on_parsed(struct http_query* q) {
    // extract method & URL
    printf("%s\n", q->url);
    http_query_destroy(q);
    return 0;
}

void http_parse(char *buf, size_t len, int (*cb)(struct http_query*)) {
    size_t i = 0;
    size_t previous_pos = 0;
    struct http_query q = {0};

    while (buf[i] != 0 && i < len-1) {
        if (buf[i] == '\r' && buf[i+1] == '\n') {
            // end of headers
            if (i == previous_pos && i != 0)
                break;

            size_t len = i-previous_pos+1;
            if (len == 0)
                break;
            char* h = malloc(len);
            if (!h)
                ERR("malloc failed")

            strncpy(h, &buf[previous_pos], len);
            h[len-1] = 0;
            list_append(&q.headers, h);

            i+=2;
            previous_pos = i;
        } else {
            i++;
        }
    }
    if (!q.headers)
        // invalid HTTP query
        return;

    q.verb = get_verb(list_access(q.headers, 0));
    if (q.verb == INVALID) {
        http_query_destroy(&q);
        return;
    }
    q.url = get_url(list_access(q.headers, 0));
    cb(&q);
}

void http_query_destroy(struct http_query *q) {
    while (q->headers != NULL)
        list_pop(&q->headers);

    if(q->url)
       free(q->url);
}

void webserv(void* s) {
    int sock = (int)(long)s;

    size_t len = 80*1024;
    char buf[len];
    ssize_t recved = recv(sock, buf, len, 0);
    if (recved < 0) {
        perror("couldn't receive whole query");
        close(sock);
        return;
    }

    http_parse(buf, recved, on_parsed);

    close(sock);
}
