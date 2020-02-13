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
    void *data;
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

err_t list_append(list_t *list, void *in_data)
{
    listnode_t *new_node = malloc(sizeof(listnode_t));
    if (!new_node) {
        return ERR_MEM_ALLOC;
    }
    new_node->data = in_data;

    /* If list is empty */
    if (!list->head) {
        list->head = new_node;
        new_node->prev = NULL;
    }
    else {
        list->tail->next = new_node;
        new_node->prev = list->tail;
    }
    list->tail = new_node;

    new_node->next = NULL;
    list->size += 1;

    return OK;
}

err_t list_remove(list_t *list, void *in_data)
{
    listnode_t *temp1 = list->head;
    listnode_t *temp2 = list->tail;

    /* If first element */
    if (list->head->data == in_data) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        free(temp1);
        list->size -= 1;
        return OK;
    }
    /* If the last element */
    else if (list->tail->data == in_data) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        free(temp2);
        list->size -= 1;
        return OK;
    }
    else {
        temp1 = temp1->next;
        while (temp1->data != in_data) {
            temp2 = temp1;
            temp1 = temp1->next;
            if (temp1 == list->tail) {
                return ERR_FAILED;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        free(temp1);
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

listnode_t *list_next(listnode_t *node)
{
    return node->next;
}

listnode_t *list_prev(listnode_t *node)
{
    return node->prev;
}

void *list_data(listnode_t *node)
{
    return node->data;
}

size_t list_size(list_t *list)
{
    return list->size;
}

void list_clean(list_t *list)
{
    listnode_t *temp1 = list->head;
    listnode_t *temp2;

    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;
        free(temp2);
        list->size -= 1;
    }
    return;
}

void list_destroy(list_t *list)
{
    list_clean(list);
    free(list);
    return;
}
