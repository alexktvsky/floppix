#include "fpx.algo.list.h"
#include "fpx.system.type.h"

#define FPX_LIST_INITIAL_SIZE  10


fpx_err_t
fpx_list_init(fpx_list_t **out_list, fpx_pool_t *pool)
{
    fpx_list_t *list;

    list = fpx_pcalloc(pool, sizeof(fpx_list_t));
    if (!list) {
        return fpx_get_errno();
    }

    /*
     * Next fields set by fpx_pcalloc()
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

    return FPX_OK;
}

fpx_err_t
fpx_list_push_back(fpx_list_t *list, void *data)
{
    fpx_list_node_t *nodes, *node, *temp;
    fpx_uint_t i;
    fpx_size_t alloc_size;

    if (list->free_nodes) {
        node = list->free_nodes;
        list->free_nodes = node->next;
        list->capacity -= 1;
    }
    else {
        alloc_size = sizeof(fpx_list_node_t) * FPX_LIST_INITIAL_SIZE;
        nodes = fpx_palloc(list->pool, alloc_size);
        if (!nodes) {
            return fpx_get_errno();
        }

        node = &nodes[0];
        nodes = &nodes[1];

        for (i = 0; i < (FPX_LIST_INITIAL_SIZE - 1); ++i) {
            temp = list->free_nodes;
            list->free_nodes = &nodes[i];
            list->free_nodes->next = temp;
        }
        list->capacity = FPX_LIST_INITIAL_SIZE - 1;
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

    return FPX_OK;
}

fpx_err_t
fpx_list_push_front(fpx_list_t *list, void *data)
{
    fpx_list_node_t *nodes, *node, *temp;
    fpx_uint_t i;
    fpx_size_t alloc_size;

    if (list->free_nodes) {
        node = list->free_nodes;
        list->free_nodes = node->next;
        list->capacity -= 1;
    }
    else {
        alloc_size = sizeof(fpx_list_node_t) * FPX_LIST_INITIAL_SIZE;
        nodes = fpx_palloc(list->pool, alloc_size);
        if (!nodes) {
            return fpx_get_errno();
        }

        node = &nodes[0];
        nodes = &nodes[1];

        for (i = 0; i < (FPX_LIST_INITIAL_SIZE - 1); ++i) {
            temp = list->free_nodes;
            list->free_nodes = &nodes[i];
            list->free_nodes->next = temp;
        }
        list->capacity = FPX_LIST_INITIAL_SIZE - 1;
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

    return FPX_OK;
}

fpx_err_t
fpx_list_reserve(fpx_list_t *list, fpx_size_t n)
{
    fpx_list_node_t *nodes, *temp;
    fpx_uint_t i;
    fpx_size_t alloc_size;

    alloc_size = sizeof(fpx_list_node_t) * n;
    nodes = fpx_palloc(list->pool, alloc_size);
    if (!nodes) {
        return fpx_get_errno();
    }

    for (i = 0; i < n; ++i) {
        temp = list->free_nodes;
        list->free_nodes = &nodes[i];
        list->free_nodes->next = temp;
    }
    list->capacity += n;

    return FPX_OK;
}

fpx_err_t
fpx_list_remove_node(fpx_list_t *list, fpx_list_node_t *node)
{
    fpx_list_node_t *founded_node, *temp1;

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
                return FPX_NOT_FOUND;
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

    return FPX_OK;
}

fpx_err_t
fpx_list_remove(fpx_list_t *list, void *data)
{
    fpx_list_node_t *founded_node, *temp1;

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
                return FPX_NOT_FOUND;
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

    return FPX_OK;
}

void
fpx_list_clear(fpx_list_t *list)
{
    fpx_list_node_t *node, *temp;

    node = list->head;

    while (node) {
        temp = node->next;
        fpx_list_remove(list, node);
        node = temp;
    }
}
