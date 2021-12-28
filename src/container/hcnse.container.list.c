#include "hcnse.container.list.h"

#define HCNSE_LIST_INITIAL_SIZE  10


hcnse_err_t
hcnse_list_create1(hcnse_list_t **out_list, hcnse_pool_t *pool)
{
    hcnse_list_t *list;

    list = hcnse_pcalloc(pool, sizeof(hcnse_list_t));
    if (!list) {
        return hcnse_get_errno();
    }

    /*
     * Next fields set by hcnse_pcalloc()
     *
     * list->head = NULL;
     * list->tail = NULL;
     * list->free_nodes = NULL;
     * list->size = 0;
     * list->capacity = 0;
     *
     */

    list->pool = pool;

    *out_list = list;

    return HCNSE_OK;
}

hcnse_list_t *
hcnse_list_create(hcnse_pool_t *pool)
{
    hcnse_list_t *list;

    if (hcnse_list_create1(&list, pool) != HCNSE_OK) {
        return NULL;
    }

    return list;
}

hcnse_err_t
hcnse_list_push_back(hcnse_list_t *list, void *data)
{
    hcnse_list_node_t *nodes, *node, *temp;
    hcnse_uint_t i;
    size_t alloc_size;

    if (list->free_nodes) {
        node = list->free_nodes;
        list->free_nodes = node->next;
        list->capacity -= 1;
    }
    else {
        alloc_size = sizeof(hcnse_list_node_t) * HCNSE_LIST_INITIAL_SIZE;
        nodes = hcnse_palloc(list->pool, alloc_size);
        if (!nodes) {
            return hcnse_get_errno();
        }

        node = &nodes[0];
        nodes = &nodes[1];

        for (i = 0; i < (HCNSE_LIST_INITIAL_SIZE - 1); ++i) {
            temp = list->free_nodes;
            list->free_nodes = &nodes[i];
            list->free_nodes->next = temp;
        }
        list->capacity = HCNSE_LIST_INITIAL_SIZE - 1;
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
hcnse_list_push_front(hcnse_list_t *list, void *data)
{
    hcnse_list_node_t *nodes, *node, *temp;
    hcnse_uint_t i;
    size_t alloc_size;

    if (list->free_nodes) {
        node = list->free_nodes;
        list->free_nodes = node->next;
        list->capacity -= 1;
    }
    else {
        alloc_size = sizeof(hcnse_list_node_t) * HCNSE_LIST_INITIAL_SIZE;
        nodes = hcnse_palloc(list->pool, alloc_size);
        if (!nodes) {
            return hcnse_get_errno();
        }

        node = &nodes[0];
        nodes = &nodes[1];

        for (i = 0; i < (HCNSE_LIST_INITIAL_SIZE - 1); ++i) {
            temp = list->free_nodes;
            list->free_nodes = &nodes[i];
            list->free_nodes->next = temp;
        }
        list->capacity = HCNSE_LIST_INITIAL_SIZE - 1;
    }

    node->prev = NULL;
    node->data = data;

    if (list->head) {
        list->head->prev = node;
        node->next = list->head;
        list->head = node;
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
    hcnse_uint_t i;
    size_t alloc_size;

    alloc_size = sizeof(hcnse_list_node_t) * n;
    nodes = hcnse_palloc(list->pool, alloc_size);
    if (!nodes) {
        return hcnse_get_errno();
    }

    for (i = 0; i < n; ++i) {
        temp = list->free_nodes;
        list->free_nodes = &nodes[i];
        list->free_nodes->next = temp;
    }
    list->capacity += n;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_remove1(hcnse_list_t *list, hcnse_list_node_t *node)
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

    list->capacity += 1;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_remove(hcnse_list_t *list, void *data)
{
    hcnse_list_node_t *founded_node, *temp1;

    temp1 = list->head;

    /* If first element */
    if (list->head->data == data) {
        founded_node = list->head;
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        list->size -= 1;
    }
    /* If the last element */
    else if (list->tail->data == data) {
        founded_node = list->tail;
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        list->size -= 1;
    }
    else {
        temp1 = temp1->next;
        while (temp1->data != data) {
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

    list->capacity += 1;

    return HCNSE_OK;
}

void
hcnse_list_clear(hcnse_list_t *list)
{
    hcnse_list_node_t *node, *temp;

    node = list->head;

    while (node) {
        temp = node->next;
        hcnse_list_remove(list, node);
        node = temp;
    }
}
