#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "list.h"


struct list_s {
    size_t size;
    listnode_t *head;
    listnode_t *tail;
};

struct listnode_s {
    listnode_t *next;
    listnode_t *prev;
};


err_t list_create(list_t **list)
{
    list_t *new_list = malloc(sizeof(list_t));
    if (!new_list) {
        return ERR_MEM_ALLOC;
    }
    memset(new_list, 0, sizeof(list_t));
    *list = new_list;
    return OK;
}

err_t list_append1(list_t *list, listnode_t *in_node)
{
    /* If list is empty */
    if (!list->head) {
        list->head = in_node;
        in_node->prev = NULL;
    }
    else {
        list->tail->next = in_node;
        in_node->prev = list->tail;
    }
    list->tail = in_node;

    in_node->next = NULL;
    list->size += 1;

    return OK;
}

err_t list_remove1(list_t *list, listnode_t *in_node)
{
    listnode_t *temp1 = list->head;

    /* If first element */
    if (list->head == in_node) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        list->size -= 1;
        return OK;
    }
    /* If the last element */
    else if (list->tail == in_node) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        list->size -= 1;
        return OK;
    }
    else {
        temp1 = temp1->next;
        while (temp1 != in_node) {
            temp1 = temp1->next;
            if (temp1 == list->tail) {
                return ERR_FAILED;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        list->size -= 1;
        return OK;
    }
}

listnode_t *list_first(list_t *list)
{
    return list->head;
}

listnode_t *list_last(list_t *list)
{
    return list->tail;
}

listnode_t *list_next1(listnode_t *node)
{
    return node->next;
}

listnode_t *list_prev1(listnode_t *node)
{
    return node->prev;
}

size_t list_size(list_t *list)
{
    return list->size;
}

void list_clean(list_t *list)
{
    memset(list, 0, sizeof(list_t));
    return;
}

void list_destroy(list_t *list)
{
    free(list);
    return;
}
