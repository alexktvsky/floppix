#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LIST_INITIAL_SIZE  10


struct hcnse_list_s {
    hcnse_pool_t *pool;
    hcnse_list_node_t *head;
    hcnse_list_node_t *tail;
    hcnse_list_node_t *free_nodes;
    size_t size;
    size_t free_nodes_size;
};


hcnse_err_t
hcnse_list_create1(hcnse_list_t **list, hcnse_pool_t *pool)
{
    hcnse_list_t *list1;

    list1 = hcnse_palloc(pool, sizeof(hcnse_list_t));
    if (!list1) {
        return hcnse_get_errno();
    }

    hcnse_memzero(list1, sizeof(hcnse_list_t));
    list1->pool = pool;

    *list = list1;
    return HCNSE_OK;
}

hcnse_list_t *
hcnse_list_create(hcnse_pool_t *pool)
{
    hcnse_list_t *list1;

    list1 = hcnse_palloc(pool, sizeof(hcnse_list_t));
    if (!list1) {
        return NULL;
    }

    hcnse_memzero(list1, sizeof(hcnse_list_t));
    list1->pool = pool;

    return list1;
}

hcnse_err_t
hcnse_list_push_back(hcnse_list_t *list, void *data)
{
    hcnse_list_node_t *nodes, *node, *temp;
    size_t alloc_size;

    if (list->free_nodes) {
        node = list->free_nodes;
        list->free_nodes = node->next;
        list->free_nodes_size -= 1;
    }
    else {
        alloc_size = sizeof(hcnse_list_node_t) * HCNSE_LIST_INITIAL_SIZE;
        nodes = hcnse_palloc(list->pool, alloc_size);
        if (!nodes) {
            return hcnse_get_errno();
        }

        node = &nodes[0];
        nodes = &nodes[1];

        for (size_t i = 0; i < (HCNSE_LIST_INITIAL_SIZE - 1); i++) {
            temp = list->free_nodes;
            list->free_nodes = &nodes[i];
            list->free_nodes->next = temp;
        }
        list->free_nodes_size = HCNSE_LIST_INITIAL_SIZE - 1;
    }

    node->next = NULL;
    node->data = data;

    if (list->tail) {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    else {
        list->head = node;
        list->tail = node;
    }

    list->size += 1;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_reserve(hcnse_list_t *list, size_t n)
{
    hcnse_list_node_t *nodes, *temp;
    size_t alloc_size;

    alloc_size = sizeof(hcnse_list_node_t) * n;
    nodes = hcnse_palloc(list->pool, alloc_size);
    if (!nodes) {
        return hcnse_get_errno();
    }

    for (size_t i = 0; i < n; i++) {
        temp = list->free_nodes;
        list->free_nodes = &nodes[i];
        list->free_nodes->next = temp;
    }
    list->free_nodes_size += n;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_remove(hcnse_list_t *list, hcnse_list_node_t *node)
{
    hcnse_list_node_t *founded_node, *temp1;

    temp1 = list->head;

    /* If first element */
    if (list->head == node) {
        founded_node = list->head;
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        list->size -= 1;
    }
    /* If the last element */
    else if (list->tail == node) {
        founded_node = list->tail;
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        list->size -= 1;
    }
    else {
        temp1 = temp1->next;
        while (temp1 != node) {
            temp1 = temp1->next;
            if (temp1 == list->tail) {
                return HCNSE_NOT_FOUND;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        founded_node = temp1;
        list->size -= 1;
    }

    temp1 = list->free_nodes;
    list->free_nodes = founded_node;
    founded_node->next = temp1;

    list->free_nodes_size += 1;

    return HCNSE_OK;
}

hcnse_list_node_t *
hcnse_list_first(hcnse_list_t *list)
{
    if (list->head) {
        return list->head;
    }
    else {
        return NULL;
    }
}

hcnse_list_node_t *
hcnse_list_last(hcnse_list_t *list)
{
    if (list->tail) {
        return list->tail;
    }
    else {
        return NULL;
    }
}

hcnse_list_node_t *
hcnse_list_next(hcnse_list_node_t *node)
{
    if (node->next) {
        return node->next;
    }
    else {
        return NULL;        
    }
}

hcnse_list_node_t *
hcnse_list_prev(hcnse_list_node_t *node)
{
    if (node->prev) {
        return node->prev;
    }
    else {
        return NULL;
    }
}

void *
hcnse_list_data1(hcnse_list_node_t *node)
{
    return node->data;
}

size_t
hcnse_list_size(hcnse_list_t *list)
{
    return list->size;
}

size_t
hcnse_list_available_size(hcnse_list_t *list)
{
    return list->free_nodes_size;
}

void
hcnse_list_clean(hcnse_list_t *list)
{
    hcnse_list_node_t *node, *temp;

    node = list->head;

    while (node) {
        temp = node->next;
        hcnse_list_remove(list, node);
        node = temp;
    }
}
