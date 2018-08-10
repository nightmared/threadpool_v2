#ifndef LIST_H_FILE
#define LIST_H_FILE

#define ERR(x) do { perror(x); exit(1); } while(0);
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list {
    void *data;
    size_t data_size;
    size_t len;
    size_t allocated_size;
};

struct list list_new(size_t allocated_slots, size_t data_size);
void list_pop(struct list *lst);
void list_append(struct list *lst, void *data);
void* list_access(struct list *lst, size_t num);
void list_free(struct list *lst);

#endif //LIST_H_FILE
