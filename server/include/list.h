#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

#include "errors.h"

#include <stdint.h>
#include <stddef.h>

#define list_cast_ptr(type, node) \
    ((type *) (((void *) node) + listnode_t_size))

#define list_cast_data(type, node) \
    (*((type *) (((void *) node) + listnode_t_size)))

#define list_cast_node_by_ptr(addr) \
    ((listnode_t *) (((void *) addr) - listnode_t_size))
    
#define list_create_node_and_append(type, list) \
    list_cast_ptr(type, list_create_node_and_append1(sizeof(type), list))


typedef struct list_s list_t;
typedef struct listnode_s listnode_t;

extern const size_t listnode_t_size;

list_t *list_create(void);
err_t list_create1(list_t **list);
err_t list_append(list_t *list, listnode_t *in_node);
err_t list_remove(list_t *list, listnode_t *in_node);
size_t list_size(list_t *list);
listnode_t *list_first(list_t *list);
listnode_t *list_last(list_t *list);
listnode_t *list_next(listnode_t *node);
listnode_t *list_prev(listnode_t *node);
void list_clean(list_t *list);
void list_destroy(list_t *list);


listnode_t *list_create_node(size_t size);
err_t list_create_node1(listnode_t **node, size_t size);
void list_destroy_node(listnode_t *node);

listnode_t *list_create_node_and_append1(size_t size, list_t *list);
void list_remove_and_destroy_node(list_t *list, listnode_t *node);

#endif /* INCLUDED_LIST_H */
