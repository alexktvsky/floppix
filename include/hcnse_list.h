#ifndef INCLUDED_HCNSE_LIST_H
#define INCLUDED_HCNSE_LIST_H

#include "hcnse_portable.h"
#include "hcnse_common.h"

struct hcnse_list_node_s {
    hcnse_list_node_t *next;
    hcnse_list_node_t *prev;
    void *data;
};

hcnse_list_t *hcnse_list_create(hcnse_pool_t *pool);
hcnse_err_t hcnse_list_create1(hcnse_list_t **list, hcnse_pool_t *pool);

hcnse_err_t hcnse_list_push_back(hcnse_list_t *list, void *data);
hcnse_err_t hcnse_list_remove(hcnse_list_t *list, hcnse_list_node_t *node);

hcnse_err_t hcnse_list_reserve(hcnse_list_t *list, size_t n);

hcnse_list_node_t *hcnse_list_first(hcnse_list_t *list);
hcnse_list_node_t *hcnse_list_last(hcnse_list_t *list);
hcnse_list_node_t *hcnse_list_next(hcnse_list_node_t *node);
hcnse_list_node_t *hcnse_list_prev(hcnse_list_node_t *node);
void *hcnse_list_data1(hcnse_list_node_t *node);

size_t hcnse_list_size(hcnse_list_t *list);
size_t hcnse_list_available_size(hcnse_list_t *list);

void hcnse_list_clean(hcnse_list_t *list);

#endif /* INCLUDED_HCNSE_LIST_H */
