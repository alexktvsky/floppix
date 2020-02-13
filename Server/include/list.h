#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

typedef struct list_s list_t;
typedef struct listnode_s listnode_t;


err_t list_create(list_t **list);
err_t list_append(list_t *list, void *in_data);
err_t list_remove(list_t *list, void *in_data);
void *list_data(listnode_t *node);
size_t list_size(list_t *list);
listnode_t *list_first(list_t *list);
listnode_t *list_last(list_t *list);
listnode_t *list_next(listnode_t *node);
listnode_t *list_prev(listnode_t *node);
void list_clean(list_t *list);
void list_destroy(list_t *list);

#endif /* INCLUDED_LIST_H */
