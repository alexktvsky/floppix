#include <stdlib.h>
#include <string.h>

#include "os/memory.h"
#include "server/list.h"


struct hcnse_list_s {
    size_t size;
    hcnse_lnode_t *head;
    hcnse_lnode_t *tail;
};

struct hcnse_lnode_s {
    hcnse_lnode_t *next;
    hcnse_lnode_t *prev;
};


const size_t hcnse_lnode_t_size = sizeof(hcnse_lnode_t);


hcnse_list_t *
hcnse_list_create(void)
{
    hcnse_list_t *new_list = hcnse_alloc(sizeof(hcnse_list_t));
    if (!new_list) {
        return NULL;
    }
    memset(new_list, 0, sizeof(hcnse_list_t));
    return new_list;
}

hcnse_err_t
hcnse_list_create1(hcnse_list_t **list)
{
    hcnse_list_t *new_list = hcnse_alloc(sizeof(hcnse_list_t));
    if (!new_list) {
        return EXIT_FAILURE;
    }
    memset(new_list, 0, sizeof(hcnse_list_t));
    *list = new_list;
    return EXIT_SUCCESS;
}

hcnse_err_t
hcnse_list_append(hcnse_list_t *list, hcnse_lnode_t *in_node)
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

hcnse_err_t
hcnse_list_remove(hcnse_list_t *list, hcnse_lnode_t *in_node)
{
    hcnse_lnode_t *temp1 = list->head;

    /* If first element */
    if (list->head == in_node) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        // hcnse_free(temp1);
        list->size -= 1;
        return EXIT_SUCCESS;
    }
    /* If the last element */
    else if (list->tail == in_node) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        // hcnse_free(temp2);
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
        // hcnse_free(temp1);
        list->size -= 1;
        return EXIT_SUCCESS;
    }
}

hcnse_lnode_t *
hcnse_list_first(hcnse_list_t *list)
{
    return list->head;
}

hcnse_lnode_t *
hcnse_list_last(hcnse_list_t *list)
{
    return list->tail;
}

hcnse_lnode_t *
hcnse_list_next(hcnse_lnode_t *node)
{
    return node->next;
}

hcnse_lnode_t *
hcnse_list_prev(hcnse_lnode_t *node)
{
    return node->prev;
}

size_t
hcnse_list_size(hcnse_list_t *list)
{
    return list->size;
}

void
hcnse_list_clean(hcnse_list_t *list)
{
    memset(list, 0, sizeof(hcnse_list_t));
    return;
}

void
hcnse_list_destroy(hcnse_list_t *list)
{
    hcnse_free(list);
}


hcnse_lnode_t *
hcnse_list_create_node(size_t size)
{
    void *mem = hcnse_alloc(sizeof(hcnse_lnode_t) + size);
    if (!mem) {
        return NULL;
    }
    ((hcnse_lnode_t *) mem)->next = NULL;
    ((hcnse_lnode_t *) mem)->prev = NULL;
    return mem;
}

hcnse_err_t
hcnse_list_create_node1(hcnse_lnode_t **node, size_t size)
{
    void *mem = hcnse_alloc(sizeof(hcnse_lnode_t) + size);
    if (!mem) {
        return EXIT_FAILURE;
    }
    ((hcnse_lnode_t *) mem)->next = NULL;
    ((hcnse_lnode_t *) mem)->prev = NULL;
    *node = mem;
    return EXIT_SUCCESS;
}

void
hcnse_list_destroy_node(hcnse_lnode_t *node)
{
    hcnse_free(node);
    return;
}

hcnse_lnode_t *
hcnse_list_create_node_and_append1(size_t size, hcnse_list_t *list)
{
    hcnse_lnode_t *node = hcnse_list_create_node(size);
    if (!node) {
        return NULL;
    }
    if (hcnse_list_append(list, node) != EXIT_SUCCESS) {
        hcnse_list_destroy_node(node);
        return NULL;
    }
    return node;
}

void
hcnse_list_remove_and_destroy_node(hcnse_list_t *list, hcnse_lnode_t *node)
{
    hcnse_list_remove(list, node);
    hcnse_list_destroy_node(node);
    return;
}

hcnse_lnode_t *
hcnse_try_use_already_exist_node1(size_t size, hcnse_list_t *free_nodes,
    hcnse_list_t *list)
{
    hcnse_lnode_t *node;

    /* Check for empty already exist node */
    node = hcnse_list_first(free_nodes);

    /* Use already exist node */
    if (node) {
        hcnse_list_remove(free_nodes, node);
        hcnse_list_append(list, node);
    }
    /* If there are not empty nodes, create new */
    else {
        node = hcnse_list_create_node(size);
        if (!node) {
            goto failed;
        }
        hcnse_list_append(list, node);
    }

    return node;

failed:
    return NULL;
}

void
hcnse_list_reserve_node(void *instance, hcnse_list_t *free_nodes,
    hcnse_list_t *list)
{
    hcnse_lnode_t *node;
    node = hcnse_list_cast_node_by_ptr(instance);
    hcnse_list_remove(list, node);
    hcnse_list_append(free_nodes, node);
}
