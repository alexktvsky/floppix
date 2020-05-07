#ifndef INCLUDED_HCNSE_LIST_H
#define INCLUDED_HCNSE_LIST_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_node_size(size) \
    (hcnse_node_t_size + size)
#define hcnse_list_push_back hcnse_list_append

extern size_t hcnse_node_t_size;

hcnse_err_t hcnse_list_init(hcnse_list_t *list);
hcnse_list_t *hcnse_list_create(void);
hcnse_err_t hcnse_list_create1(hcnse_list_t **list);
hcnse_err_t hcnse_list_append(hcnse_list_t *list, void *ptr);
hcnse_err_t hcnse_list_remove(hcnse_list_t *list, void *ptr);
size_t hcnse_list_size(hcnse_list_t *list);
void *hcnse_list_first(hcnse_list_t *list);
void *hcnse_list_last(hcnse_list_t *list);
void *hcnse_list_next(void *ptr);
void *hcnse_list_prev(void *ptr);
void hcnse_list_clean(hcnse_list_t *list);
void hcnse_list_destroy(hcnse_list_t *list);

void *hcnse_list_init_node(void *mem);
void *hcnse_list_create_node(size_t size);
hcnse_err_t hcnse_list_create_node1(void **ptr, size_t size);
void hcnse_list_destroy_node(void *ptr);

void *hcnse_list_init_node_and_append(void *mem, hcnse_list_t *list);
void *hcnse_list_create_node_and_append(size_t size, hcnse_list_t *list);
void hcnse_list_remove_and_destroy_node(hcnse_list_t *list, void *ptr);
void *hcnse_try_use_already_exist_node(size_t size, hcnse_list_t *fnodes,
    hcnse_list_t *list);
void *hcnse_try_use_already_exist_node1(void *mem, hcnse_list_t *free_nodes,
    hcnse_list_t *list);
void hcnse_list_reserve_node(void *instance, hcnse_list_t *free_nodes,
    hcnse_list_t *list);

#endif /* INCLUDED_HCNSE_LIST_H */
