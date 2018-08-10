#include "threadpool.h"
#include <signal.h>

#define NUMBER_THREADS 4

struct web_server {
    int web_socket;
    int polling_fd;
    struct thread_list threads;
};

static int web_socket, epfd;
static struct thread_list threads;

void cleanup_handler(int _) {
    thread_list_destroy(&threads);
    close(epfd);
    close(web_socket);
    parser_cleanup();
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    // We MUST call this before starting any other thread to prevent any risk of race condition
    parser_prepare();

    web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == 0)
        ERR("Couldn't allocate a socket")

    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(8080);
    web_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(web_socket, (struct sockaddr*)&web_addr, sizeof(struct sockaddr_in)))
        ERR("bind failed")

    if (listen(web_socket, 4096))
        ERR("listen failed")

    threads = thread_list_new(NUMBER_THREADS, threadpool_handler);
    epfd = thread_list_create_epoll_queue(&threads);

    // close the socket in the event of interruption from user
    struct sigaction sig;
    sig.sa_handler = cleanup_handler;
    sigaction(SIGINT, &sig, NULL);

    struct epoll_event ev;
    epoll_data_t val;
    val.u64 = 0;
    ev.data = val;
    ev.events = EPOLLIN;
    int res = epoll_ctl(epfd, EPOLL_CTL_ADD, web_socket, &ev);
    if (res)
        ERR("epoll_ctl failed")

    int running = NUMBER_THREADS;
    // an event per thread plus an avent for the listening socket
    struct epoll_event events[NUMBER_THREADS+1];
    while (running) {
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
                    thread_list_schedule_work(&threads, NULL);
                }
            } else {
                // This is the web server socket
                struct sockaddr_in remote_addr;
                socklen_t remote_addr_len;
                long sock = accept(web_socket, (struct sockaddr*)&remote_addr, &remote_addr_len);
                if (sock == 0)
                    ERR("accept failed")

                thread_list_schedule_work(&threads, (void*)sock);
            }
        }
    }

    cleanup_handler(0);
}
