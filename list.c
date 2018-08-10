#include "list.h"

size_t next_multiple_of_two(size_t num) {
    size_t res = 32;
    while (res < num)
        res*=2;
    return res;
}

struct list list_new(size_t allocated_slots, size_t data_size) {
    struct list lst;
    lst.data_size = data_size;
    lst.len = 0;
    lst.allocated_size = next_multiple_of_two(allocated_slots);
    lst.data = calloc(lst.allocated_size, lst.data_size);
    if (!lst.data)
        ERR("calloc failed !")
    return lst;
}

void list_pop(struct list *lst) {
    if (lst->len)
        lst->len -= 1;
}

void list_append(struct list *lst, void *data) {
    if (lst->len+1 > lst->allocated_size) {
        lst->allocated_size *= 2;
        lst->data = realloc(lst->data, lst->allocated_size);
        if (!lst->data)
            ERR("realloc failed")
    }
    lst->len += 1;
    memcpy(lst->data+(lst->len-1)*lst->data_size, data, lst->data_size);
}

void* list_access(struct list *lst, size_t num) {
    if (num > lst->len)
        ERR("out of bounds access !")
    return lst->data+num*lst->data_size;
}

void list_free(struct list *lst) {
    // prevent any illegal access later
    lst->len = 0;
    free(lst->data);
}
