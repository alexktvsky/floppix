#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

#include "server/errors.h"

#include <stdint.h>
#include <stddef.h>

#define hcnse_list_cast_ptr(type, node) \
    ((type *) (((void *) node) + hcnse_lnode_t_size))

#define hcnse_list_cast_data(type, node) \
    (*((type *) (((void *) node) + hcnse_lnode_t_size)))

#define hcnse_list_cast_node_by_ptr(addr) \
    ((hcnse_lnode_t *) (((void *) addr) - hcnse_lnode_t_size))
    
#define hcnse_list_create_node_and_append(type, list) \
    hcnse_list_cast_ptr(type, \
        hcnse_list_create_node_and_append1(sizeof(type), list))

#define hcnse_try_use_already_exist_node(type, fnodes, list) \
    hcnse_list_cast_ptr(type, \
        hcnse_try_use_already_exist_node1(sizeof(type), fnodes, list))


#define hcnse_list_push_back hcnse_list_append


typedef struct hcnse_list_s hcnse_list_t;
typedef struct hcnse_lnode_s hcnse_lnode_t;

extern const size_t hcnse_lnode_t_size;

hcnse_list_t *hcnse_list_create(void);
hcnse_err_t hcnse_list_create1(hcnse_list_t **list);
hcnse_err_t hcnse_list_append(hcnse_list_t *list, hcnse_lnode_t *in_node);
hcnse_err_t hcnse_list_remove(hcnse_list_t *list, hcnse_lnode_t *in_node);
size_t hcnse_list_size(hcnse_list_t *list);
hcnse_lnode_t *hcnse_list_first(hcnse_list_t *list);
hcnse_lnode_t *hcnse_list_last(hcnse_list_t *list);
hcnse_lnode_t *hcnse_list_next(hcnse_lnode_t *node);
hcnse_lnode_t *hcnse_list_prev(hcnse_lnode_t *node);
void hcnse_list_clean(hcnse_list_t *list);
void hcnse_list_destroy(hcnse_list_t *list);


hcnse_lnode_t *hcnse_list_create_node(size_t size);
hcnse_err_t hcnse_list_create_node1(hcnse_lnode_t **node, size_t size);
void hcnse_list_destroy_node(hcnse_lnode_t *node);

hcnse_lnode_t *hcnse_list_create_node_and_append1(size_t size,
    hcnse_list_t *list);
void hcnse_list_remove_and_destroy_node(hcnse_list_t *list,
    hcnse_lnode_t *node);
hcnse_lnode_t *hcnse_try_use_already_exist_node1(size_t size,
    hcnse_list_t *fnodes, hcnse_list_t *list);
void hcnse_list_reserve_node(void *instance, hcnse_list_t *free_nodes,
    hcnse_list_t *list);

#endif /* INCLUDED_LIST_H */
