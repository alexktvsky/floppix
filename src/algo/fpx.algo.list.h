#ifndef FPX_ALGO_LIST_H
#define FPX_ALGO_LIST_H

#include "fpx.core.forwards.h"
#include "fpx.system.errno.h"
#include "fpx.system.os.portable.h"
#include "fpx.system.pool.h"

typedef struct fpx_list_node_s fpx_list_node_t;

struct fpx_list_node_s {
    fpx_list_node_t *next;
    fpx_list_node_t *prev;
    void *data;
};

typedef struct fpx_list_s {
    fpx_pool_t *pool;
    fpx_list_node_t *head;
    fpx_list_node_t *tail;
    fpx_list_node_t *free_nodes;
    size_t size;
    size_t capacity;
} fpx_list_t;


fpx_err_t fpx_list_init(fpx_list_t **list, fpx_pool_t *pool);
fpx_err_t fpx_list_push_back(fpx_list_t *list, void *data);
fpx_err_t fpx_list_push_front(fpx_list_t *list, void *data);
fpx_err_t fpx_list_remove(fpx_list_t *list, void *data);
fpx_err_t fpx_list_remove_node(fpx_list_t *list, fpx_list_node_t *node);
fpx_err_t fpx_list_reserve(fpx_list_t *list, size_t n);
void fpx_list_clear(fpx_list_t *list);

#endif /* FPX_ALGO_LIST_H */
