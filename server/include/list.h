#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

typedef struct list_s list_t;
typedef struct listnode_s listnode_t;

#define list_append(list, in_node) \
    (list_append1(list, (listnode_t *) in_node))

#define list_next(in_node) \
    (list_next1((listnode_t *) in_node))

#define list_prev(in_node) \
    (list_prev1((listnode_t *) in_node))

#define list_remove(list, in_node) \
    (list_remove1(list, (listnode_t *) in_node))

#define list_begin(list) list_first(list)
#define list_end(list) list_last(list)

#define list_foreach(type, var, list) \
    for (type var = (type) list_begin(list); \
        var != NULL; var = (type) list_next(var))

err_t list_create(list_t **list);
err_t list_append1(list_t *list, listnode_t *in_node);
err_t list_remove1(list_t *list, listnode_t *in_node);
size_t list_size(list_t *list);
listnode_t *list_first(list_t *list);
listnode_t *list_last(list_t *list);
listnode_t *list_next1(listnode_t *node);
listnode_t *list_prev1(listnode_t *node);
void list_clean(list_t *list);
void list_destroy(list_t *list);


#endif /* INCLUDED_LIST_H */
