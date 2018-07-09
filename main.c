#include "threadpool.h"
#include "webserv.h"

#define NUMBER_THREADS 4

void* handler(void *val) {
    struct thread *th = (struct thread*)val;
    while(1) {
        struct query msg;
        recv_query(th->remote_socket, &msg);
        if (msg.state == Stop) {
            enum Answer ans = Stopped;
            send_answer(th->remote_socket, &ans);
            return 0;
        }
        if (msg.state == RunTask) {
            webserv(*(int*)msg.task);
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == 0)
        ERR("Couldn't allocate a socket")

    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = 8080;
    web_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(web_socket, (struct sockaddr*)&web_addr, sizeof(struct sockaddr_in)))
        ERR("bind failed")

    if (listen(web_socket, 4096))
        ERR("listen failed")

    struct thread_list threads = thread_list_new(NUMBER_THREADS, &handler);
    int epfd = thread_list_create_epoll_queue(&threads);

    struct epoll_event ev;
    epoll_data_t val;
    val.u64 = 0;
    ev.data = val;
    ev.events = EPOLLIN;
    int res = epoll_ctl(epfd, EPOLL_CTL_ADD, web_socket, &ev);
    if (res)
        ERR("epoll_ctl failed")

    int running = NUMBER_THREADS;
    struct epoll_event events[NUMBER_THREADS+1];
    while (running != 0) {
        int res = epoll_wait(epfd, events, NUMBER_THREADS+1, -1);
        if (res == -1)
            ERR("epoll_wait failed")

        for (int i = 0; i < res; i++) {
            if (events[i].data.u64 != 0) {
                struct thread *th = events[i].data.ptr;
                enum Answer r;
                recv_answer(th->socket, &r);
                if (r == Stopped) {
                    running--;
                } else if (r == TaskResult) {
                    th->available = 1;
                }
            } else {
                // This is the web server socket

            }
        }
    }

    thread_list_destroy(&threads);
    close(epfd);

    close(web_socket);
    return EXIT_SUCCESS;
}
