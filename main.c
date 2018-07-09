#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

void *handler(void *val) {
    struct thread *th = (struct thread*)val;
    printf("%i\n", th->remote_socket);
    return 0;
}

int main(int argc, char* argv[]) {
    struct thread_list threads = thread_list_new(4, &handler); 
    int epfd = thread_list_create_epoll_queue(&threads);
    thread_list_destroy(&threads);
    return EXIT_SUCCESS;
}
