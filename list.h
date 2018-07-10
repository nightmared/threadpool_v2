#ifndef LIST_H_FILE
#define LIST_H_FILE

#define ERR(x) do { perror(x); exit(1); } while(0);
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct list_node {
    void *data;
    struct list_node* next;
};

void* list_pop(struct list_node **head);
void list_append(struct list_node **head, void *data);
void* list_access(struct list_node *head, size_t pos);

#endif //LIST_H_FILE
