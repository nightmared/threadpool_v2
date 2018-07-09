#include "threadpool.h"

struct thread thread_new() {
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets))
        ERR("socket pair creation failed")

    struct thread th;
    th.socket = sockets[0];
    th.remote_socket = sockets[1];
    th.available = 0;
    th.fd = 0;

    return th;
}

void thread_run(struct thread *th, void *(*fun) (void *)) {
    if (pthread_create(&th->fd, NULL, fun, th))
        ERR("pthread_create() failed")
    th->available = 1;
}

void thread_stop(struct thread *th) {
    struct query q;
    q.state = Stop;
    q.task = NULL;
    send_query(th->socket, &q);
    th->available = 0;
}

void thread_destroy(struct thread *th) {
    pthread_cancel(th->fd);
    close(th->socket);
    close(th->remote_socket);
    th->available = 0;
}


void send_query(int sock, struct query* q) {
    int len = send(sock, q, sizeof(struct query), 0);
    if (len == -1 || len < sizeof(struct query))
        ERR("send failed or message too short")
}

void recv_query(int sock, struct query* q) {
    int len = recv(sock, q, sizeof(struct query), 0);
    if (len == -1 || len < sizeof(struct query))
        ERR("send failed or message too short")
}

void send_answer(int sock, enum Answer *q) {
    int len = send(sock, q, sizeof(enum Answer), 0);
    if (len == -1 || len < sizeof(enum Answer))
        ERR("send failed or message too short")
}

void recv_answer(int sock, enum Answer *q) {
    int len = recv(sock, q, sizeof(enum Answer), 0);
    if (len == -1 || len < sizeof(enum Answer))
        ERR("send failed or message too short")
}



struct thread_list thread_list_new(uint32_t len, void *(*fun) (void *)) {
    void* data = malloc(len*sizeof(struct thread));
    if (data == NULL)
        ERR("malloc failed")

    struct thread_list res;
    res.len = len;
    res.threads = data;
    res.fun = fun;
    res.queue = NULL;

    for (uint32_t i = 0; i < len; i++) {
        res.threads[i] = thread_new();
        thread_run(&res.threads[i], fun);
        char thread_name[15];
        snprintf((char*)&thread_name, 15, "pool-%i", i);
        pthread_setname_np(res.threads[i].fd, (char*)&thread_name);
    }

    return res;
}

void thread_list_respawn(struct thread_list *p, uint32_t pos) {
    if (pos >= p->len)
        return;

    thread_destroy(&p->threads[pos]);
    p->threads[pos] = thread_new();
    thread_run(&p->threads[pos], p->fun);
}

int thread_list_create_epoll_queue(struct thread_list *p) {
    int epfd = epoll_create1(0);
    if (epfd == 0)
        ERR("epoll_create failed")

    for (uint32_t i = 0; i < p->len; i++) {
        struct epoll_event ev;
        epoll_data_t val;
        val.ptr = &p->threads[i];
        ev.data = val;
        ev.events = EPOLLIN;
        int res = epoll_ctl(epfd, EPOLL_CTL_ADD, p->threads[i].socket, &ev);
        if (res)
            ERR("epoll_ctl failed")
    }

    return epfd;
}

void* thread_list_wait_queue_pop(struct thread_list *p) {
    if (p->queue == NULL)
        ERR("pop called on a free list")

    void *ptr = p->queue->data;
    void *next = p->queue->next;
    free(p->queue);
    p->queue = next;
    return ptr;
}

void thread_list_wait_queue_append(struct thread_list *p, void *data) {
    struct wait_node *next = p->queue;
    if (next != NULL) {
        while (next->next != NULL) {
            next = next->next;
        }
        next = next->next;
    }
    next = malloc(sizeof(struct wait_node));
    if (next == NULL)
        ERR("malloc failed")
    next->data = data;
    next->next = NULL;
}

void thread_list_schedule_work(struct thread_list *p, void *task) {
    for (int i = 0; i < p-> len; i++) {
        if (p->threads[i].available) {
            p->threads[i].available = 0;
            struct query q;
            q.state = RunTask;

            // priorize elements already in the queue
            if (p->queue != NULL) {
                void *queued_task = thread_list_wait_queue_pop(p);
                q.task = queued_task;
            } else {
                q.task = task;
            }

            send_query(p->threads[i].socket, &q);
            return;
        }
    }
    // no thread available
    thread_list_wait_queue_append(p, task);
}

void thread_list_stop(struct thread_list *p) {
    for (uint32_t i = 0; i < p->len; i++) {
        thread_stop(&p->threads[i]);
    }
}

void thread_list_destroy(struct thread_list *p) {
    for (uint32_t i = 0; i < p->len; i++) {
        thread_destroy(&p->threads[i]);
    }
    free(p->threads);
}
