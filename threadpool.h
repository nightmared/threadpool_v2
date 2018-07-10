#ifndef THREADPOOL_H_FILE
#define THREADPOOL_H_FILE

// neccessary for 'pthread_setname_np'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "list.h"
#include "webserv.h"

enum QueryState {
    RunTask,
    Stop
};


struct query {
    enum QueryState state;
    void* task;
};

enum Answer {
    TaskResult,
    Stopped
};

struct thread {
    int socket;
    int remote_socket;
    int available;
    pthread_t fd;
};

struct thread thread_new();
void thread_run(struct thread *th, void *(*fun) (void *));
void thread_stop(struct thread *th);
void thread_destroy(struct thread *th);

void send_query(int socket, struct query *q);
void recv_query(int socket, struct query *q);
void send_answer(int socket, enum Answer *r);
void recv_answer(int socket, enum Answer *r);


struct thread_list {
    uint32_t len;
    struct thread* threads;
    void *(*fun) (void *);
    struct list_node *queue;
};

struct thread_list thread_list_new(uint32_t len, void *(*fun) (void *));
// Respawn the thread number pos
void thread_list_respawn(struct thread_list *p, uint32_t pos);
// Create an epoll queue to listen for threads
int thread_list_create_epoll_queue(struct thread_list *p);
void thread_list_schedule_work(struct thread_list *p, void *task);
// Send a Stop query to every thread in the list
void thread_list_stop(struct thread_list *p);
void thread_list_destroy(struct thread_list*);

void* threadpool_handler(void *val);

#endif //THREADPOOL_H_FILE
