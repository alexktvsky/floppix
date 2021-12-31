#ifndef HCNSE_CONTAINER_LIST_H
#define HCNSE_CONTAINER_LIST_H

#include "hcnse.type.forwards.h"
#include "hcnse.system.errno.h"
#include "hcnse.system.os.portable.h"
#include "hcnse.system.pool.h"

typedef struct hcnse_list_node_s hcnse_list_node_t;

struct hcnse_list_node_s {
    hcnse_list_node_t *next;
    hcnse_list_node_t *prev;
    void *data;
};

typedef struct hcnse_list_s {
    hcnse_pool_t *pool;
    hcnse_list_node_t *head;
    hcnse_list_node_t *tail;
    hcnse_list_node_t *free_nodes;
    size_t size;
    size_t capacity;
} hcnse_list_t;


hcnse_err_t hcnse_list_init(hcnse_list_t **list, hcnse_pool_t *pool);
hcnse_err_t hcnse_list_push_back(hcnse_list_t *list, void *data);
hcnse_err_t hcnse_list_push_front(hcnse_list_t *list, void *data);
hcnse_err_t hcnse_list_remove(hcnse_list_t *list, void *data);
hcnse_err_t hcnse_list_remove_node(hcnse_list_t *list, hcnse_list_node_t *node);
hcnse_err_t hcnse_list_reserve(hcnse_list_t *list, size_t n);
void hcnse_list_clear(hcnse_list_t *list);

#endif /* HCNSE_CONTAINER_LIST_H */
