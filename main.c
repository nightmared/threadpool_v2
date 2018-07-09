#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

#define NUMBER_THREADS 4

void *handler(void *val) {
    struct thread *th = (struct thread*)val;
    while(1) {
        struct query msg;
        recv_query(th->remote_socket, &msg);
        if (msg.state == Stop) {
            struct answer ans;
            ans.state = Stopped;
            ans.res = 0;
            send_answer(th->remote_socket, &ans);
            return 0;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    struct thread_list threads = thread_list_new(NUMBER_THREADS, &handler);
    int epfd = thread_list_create_epoll_queue(&threads);

    thread_list_stop(&threads);
    int running = NUMBER_THREADS;
    struct epoll_event events[NUMBER_THREADS+1];
    while (running != 0) {
        int res = epoll_wait(epfd, events, NUMBER_THREADS+1, -1);
        if (res == -1)
            ERR("epoll_wait failed")

        for (int i = 0; i < res; i++) {
            struct thread *th = events[i].data.ptr;
            struct answer r;
            recv_answer(th->socket, &r);
            if (r.state == Stopped)
                running--;
        }
    }

    thread_list_destroy(&threads);
    return EXIT_SUCCESS;
}
