#ifndef FLOPPIX_ALGO_LIST_H
#define FLOPPIX_ALGO_LIST_H

#include "floppix/core/forwards.h"
#include "floppix/system/errno.h"
#include "floppix/system/os/portable.h"
#include "floppix/system/memory.h"

#define fpx_list_data(ptr, type, member) fpx_container_of(ptr, type, member)

#define fpx_list_foreach(pos, list)                                            \
    for (fpx_list_node_t *pos = fpx_list_head(list); pos != NULL;              \
         pos = fpx_list_next(pos))

#define fpx_list_foreach_reverse(pos, list)                                    \
    for (fpx_list_node_t *pos = fpx_list_tail(list); pos != NULL;              \
         pos = fpx_list_prev(pos))

#define fpx_list_cleanup(list, type, member, callback)                         \
    for (fpx_list_node_t *__pos = NULL; (__pos = fpx_list_head(list1));) {     \
        fpx_list_remove(list1, __pos);                                         \
        type *__ptr = fpx_list_data(__pos, type, member);                      \
        callback(__ptr);                                                       \
    }

#ifndef FPX_DEBUG

#define fpx_list_next(node) ((node)->next)

#define fpx_list_prev(node) ((node)->prev)

#define fpx_list_size(list) ((list)->size)

#define fpx_list_head(list) ((list)->head)

#define fpx_list_tail(list) ((list)->tail)

#endif /* FPX_DEBUG */

typedef struct fpx_listnode_s fpx_list_node_t;

struct fpx_listnode_s {
    fpx_list_node_t *next;
    fpx_list_node_t *prev;
};

typedef struct fpx_list_s fpx_list_t;

struct fpx_list_s {
    fpx_list_node_t *head;
    fpx_list_node_t *tail;
    size_t size;
};

void fpx_list_init(fpx_list_t *list);
void fpx_list_push_front(fpx_list_t *list, fpx_list_node_t *node);
void fpx_list_push_back(fpx_list_t *list, fpx_list_node_t *node);
fpx_list_node_t *fpx_list_find(fpx_list_t *list,
    bool (*compar)(const fpx_list_node_t *));
void fpx_list_insert_before(fpx_list_t *list, fpx_list_node_t *before_node,
    fpx_list_node_t *node);
void fpx_list_insert_after(fpx_list_t *list, fpx_list_node_t *after_node,
    fpx_list_node_t *node);
fpx_err_t fpx_list_remove_first(fpx_list_t *list,
    bool (*compar)(const fpx_list_node_t *));
fpx_err_t fpx_list_remove_last(fpx_list_t *list,
    bool (*compar)(const fpx_list_node_t *));
void fpx_list_mergesort(fpx_list_t *list,
    int (*compar)(const fpx_list_node_t *, const fpx_list_node_t *));
void fpx_list_remove(fpx_list_t *list, fpx_list_node_t *node);
void fpx_list_clear(fpx_list_t *list);

#ifdef FPX_DEBUG

static inline fpx_list_node_t *fpx_list_next(fpx_list_node_t *node);
static inline fpx_list_node_t *fpx_list_prev(fpx_list_node_t *node);
static inline size_t fpx_list_size(fpx_list_t *list);
static inline fpx_list_node_t *fpx_list_head(fpx_list_t *list);
static inline fpx_list_node_t *fpx_list_tail(fpx_list_t *list);

static inline fpx_list_node_t *
fpx_list_next(fpx_list_node_t *node)
{
    return node->next;
}

static inline fpx_list_node_t *
fpx_list_prev(fpx_list_node_t *node)
{
    return node->prev;
}

static inline size_t
fpx_list_size(fpx_list_t *list)
{
    return list->size;
}

static inline fpx_list_node_t *
fpx_list_head(fpx_list_t *list)
{
    return list->head;
}

static inline fpx_list_node_t *
fpx_list_tail(fpx_list_t *list)
{
    return list->tail;
}

#endif /* FPX_DEBUG */

#endif /* FLOPPIX_ALGO_LIST_H */
