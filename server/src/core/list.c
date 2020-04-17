#include <stdlib.h>
#include <string.h>

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


const size_t listnode_t_size = sizeof(listnode_t);


list_t *list_create(void)
{
    list_t *new_list = malloc(sizeof(list_t));
    if (!new_list) {
        return NULL;
    }
    memset(new_list, 0, sizeof(list_t));
    return new_list;
}

err_t list_create1(list_t **list)
{
    list_t *new_list = malloc(sizeof(list_t));
    if (!new_list) {
        return EXIT_FAILURE;
    }
    memset(new_list, 0, sizeof(list_t));
    *list = new_list;
    return EXIT_SUCCESS;
}

err_t list_append(list_t *list, listnode_t *in_node)
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

    return EXIT_SUCCESS;
}

err_t list_remove(list_t *list, listnode_t *in_node)
{
    listnode_t *temp1 = list->head;

    /* If first element */
    if (list->head == in_node) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        // free(temp1);
        list->size -= 1;
        return EXIT_SUCCESS;
    }
    /* If the last element */
    else if (list->tail == in_node) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        // free(temp2);
        list->size -= 1;
        return EXIT_SUCCESS;
    }
    else {
        temp1 = temp1->next;
        while (temp1 != in_node) {
            temp1 = temp1->next;
            if (temp1 == list->tail) {
                return EXIT_FAILURE;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        // free(temp1);
        list->size -= 1;
        return EXIT_SUCCESS;
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


listnode_t *list_create_node(size_t size)
{
    void *mem = malloc(sizeof(listnode_t) + size);
    if (!mem) {
        return NULL;
    }
    ((listnode_t *) mem)->next = NULL;
    ((listnode_t *) mem)->prev = NULL;
    return mem;
}

err_t list_create_node1(listnode_t **node, size_t size)
{
    void *mem = malloc(sizeof(listnode_t) + size);
    if (!mem) {
        return EXIT_FAILURE;
    }
    ((listnode_t *) mem)->next = NULL;
    ((listnode_t *) mem)->prev = NULL;
    *node = mem;
    return EXIT_SUCCESS;
}

void list_destroy_node(listnode_t *node)
{
    free(node);
    return;
}

listnode_t *list_create_node_and_append1(size_t size, list_t *list)
{
    listnode_t *node = list_create_node(size);
    if (!node) {
        return NULL;
    }
    if (list_append(list, node) != EXIT_SUCCESS) {
        list_destroy_node(node);
        return NULL;
    }
    return node;
}

void list_remove_and_destroy_node(list_t *list, listnode_t *node)
{
    list_remove(list, node);
    list_destroy_node(node);
    return;
}

listnode_t *try_use_already_exist_node1(size_t size, list_t *free_nodes, list_t *list)
{
    listnode_t *node;

    /* Check for empty already exist node */
    node = list_first(free_nodes);

    /* Use already exist node */
    if (node) {
        list_remove(free_nodes, node);
        list_append(list, node);
    }
    /* If there are not empty nodes, create new */
    else {
        node = list_create_node(size);
        if (!node) {
            goto failed;
        }
        list_append(list, node);
    }

    return node;

failed:
    return NULL;
}

void reserve_node(void *instance, list_t *free_nodes, list_t *list)
{
    listnode_t *node;
    node = list_cast_node_by_ptr(instance);
    list_remove(list, node);
    list_append(free_nodes, node);
}
