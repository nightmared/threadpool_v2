#include "http.h"
#include "aho_tree.h"

#define MIN(a, b) (a > b ? b : a)
#define COMPARE_RETURN(str, val, strval) if (strncmp(str, strval, strlen(strval)) == 0) \
    return val;

static struct aho_tree verb_match_tree;

void parser_prepare() {
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

void parser_cleanup() {
    tree_free(&verb_match_tree);
}

int contain(char* src, char* cmp, size_t pos) {
    size_t srclen = strlen(src);
    size_t cmplen = strlen(cmp);

    if (cmplen+pos > srclen)
        return 0;

    size_t counter = 0;
    while (counter < cmplen && src[pos+counter] == cmp[counter])
        counter++;

    if (counter == cmplen)
        return 1;
    return 0;
}

// return header_length, including CRLF
size_t get_header_length(char *buf) {
    size_t pos = 0;
    while (buf[pos] != '\0') {
        if (buf[pos] == '\r' && buf[pos+1] == '\n')
            break;
        pos++;
    }
    // include CRLF size
    return pos+2;
}

void http_parse(char *buf, size_t len, int (*cb)(struct http_query*)) {
    struct http_query q = {0};

    size_t pos = 0;

    // ignore any CLRF before the Request-Line, per the specification (https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
    while (pos < len && (buf[pos] == '\n' || buf[pos] == '\r'))
        pos++;

    // match the http verb
    struct match verb_res = tree_match(&verb_match_tree, buf+pos);
    q.verb = verb_res.node->data;
    // what's the point of going any further if we can't event match the verb ?
    if (q.verb == INVALID)
        return;
    // ignore the space after the HTTP verb
    pos += verb_res.length_matched+1;

    // Let's get the Request-URI size, it's 'req_len-11'
    size_t req_len = get_header_length(buf+pos);
    // No Request-URI !? Let's drop that packet
    if (req_len < 12)
        return;

    // let's copy Request-URI to a new buffer if this is a true HTTP Request
    if (contain(buf+pos, " HTTP/1.1", req_len-11)) {
        // Let's not forget the NULL byte
        q.url = malloc(req_len-10);
        if (!q.url)
            ERR("malloc failed")
        strncpy(q.url, buf+pos, req_len-11);
        q.url[req_len-11] = '\0';
    }
    pos += req_len;

    q.headers = list_new(8, sizeof(char*));
    // let's match all those freaking headers
    size_t offset;
    // "offset == 2" => matched the second \r\n, time to stop matching headers, we're at the body now ;)
    while ((offset = get_header_length(buf+pos)) != 2) {
        char *copy_buf = malloc(offset-1);
        if (!copy_buf)
            ERR("malloc failed !")
        list_append(&q.headers, copy_buf);
        pos += offset;
    }
    pos += 2;

    // no body at all !? Well, let's continue anyway
    if (len <= pos) {
        q.body = NULL;
    } else {
        q.body = malloc(len-pos+1);
        if (!q.body)
            ERR("malloc failed")
        memcpy(q.body, buf+pos, len-pos);
        q.body[len-pos] = '\0';
    }

    cb(&q);
    http_query_destroy(&q);
}

void http_query_destroy(struct http_query *q) {
    while (q->headers.len > 0) {
        free(*(void**)list_access(&q->headers, q->headers.len-1));
        list_pop(&q->headers);
    }
    list_free(&q->headers);

    if(q->url)
       free(q->url);

    if(q->body)
       free(q->body);
}
