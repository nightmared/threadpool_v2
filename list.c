#include "list.h"

void* list_pop(struct list_node **head) {
    if (*head == NULL)
        ERR("pop called on a free list")

    void *ptr = (*head)->data;
    void *next = (*head)->next;
    free(*head);
    *head = next;
    return ptr;
}

void list_append(struct list_node **head, void *data) {
    struct list_node **next = head;
    if (*next != NULL) {
        while ((*next)->next != NULL) {
            next = &(*next)->next;
        }
        next = &(*next)->next;
    }
    *next = malloc(sizeof(struct list_node));
    if (*next == NULL)
        ERR("malloc failed")
    (*next)->data = data;
    (*next)->next = NULL;
}

void* list_access(struct list_node *head, size_t pos) {
    struct list_node *next = head;
    if (next != NULL) {
        while (next->next != NULL && pos > 0) {
            next = next->next;
            pos -= 1;
        }
        return next->data;
    }
    return NULL;
}
