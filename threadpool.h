#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#ifndef THREADPOOL_H_FILE
#define THREADPOOL_H_FILE

#define ERR(x) do { perror(x); exit(1); } while(0);

enum QueryState {
    RunTask,
    Stop
};


struct query {
    enum QueryState state;
    void* task;
};

enum AnswerState {
    TaskResult,
    Stopped
};

struct answer {
    enum AnswerState state;
    void* res;
};

struct thread {
    int socket;
    int remote_socket;
    pthread_t fd;
};

struct thread thread_new();
void thread_run(struct thread *th, void *(*fun) (void *));
void thread_send_msg(struct thread *th, struct query* q);
void thread_recv_msg(struct thread *th, struct answer* r);
void thread_stop(struct thread *th);
void thread_destroy(struct thread *th);

struct thread_list {
    uint32_t len;
    struct thread* threads;
    void *(*fun) (void *);
};

struct thread_list thread_list_new(uint32_t len, void *(*fun) (void *));
// Respawn the thread number pos
void thread_list_respawn(struct thread_list *p, uint32_t pos);
// Create an epoll queue to listen for threads
int thread_list_create_epoll_queue(struct thread_list *p);
void thread_list_destroy(struct thread_list*);

#endif //THREADPOOL_H_FILE
