#include "floppix/algo/list.h"
#include "floppix/system/types.h"
#include "floppix/system/memory.h"

void
fpx_list_init(fpx_list_t *list)
{
    fpx_memzero(list, sizeof(fpx_list_t));
}

void
fpx_list_push_front(fpx_list_t *list, fpx_list_node_t *node)
{
    node->prev = NULL;

    if (list->head == NULL) {
        list->tail = node;
        node->next = NULL;
    }
    else {
        list->head->prev = node;
        node->next = list->head;
    }

    list->head = node;
    list->size += 1;
}

void
fpx_list_push_back(fpx_list_t *list, fpx_list_node_t *node)
{
    node->next = NULL;

    if (!list->tail) {
        list->head = node;
        node->prev = NULL;
    }
    else {
        list->tail->next = node;
        node->prev = list->tail;
    }

    list->tail = node;
    list->size += 1;
}

void
fpx_list_remove(fpx_list_t *list, fpx_list_node_t *node)
{
    if (node->next) {
        node->next->prev = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }

    if (list->head == node) {
        list->head = node->next;
    }

    if (list->tail == node) {
        list->tail = node->prev;
    }

    list->size -= 1;
}

void
fpx_list_clear(fpx_list_t *list)
{
    memset(list, 0, sizeof(fpx_list_t));
}

fpx_list_node_t *
fpx_list_find(fpx_list_t *list, bool (*compar)(const fpx_list_node_t *))
{
    fpx_list_node_t *temp1;

    temp1 = list->head;

    while (temp1) {
        if (compar(temp1)) {
            return temp1;
        }
        temp1 = temp1->next;
    }
    return NULL;
}

void
fpx_list_insert_before(fpx_list_t *list, fpx_list_node_t *before_node,
    fpx_list_node_t *node)
{
    if (!before_node->prev) {
        list->head = node;
        node->prev = NULL;
        node->next = before_node;
        before_node->prev = node;
    }
    else {
        node->next = before_node;
        node->prev = before_node->prev;
        before_node->prev->next = node;
        before_node->prev = node;
    }
}

void
fpx_list_insert_after(fpx_list_t *list, fpx_list_node_t *after_node,
    fpx_list_node_t *node)
{
    if (!after_node->next) {
        list->tail = node;
        node->prev = after_node;
        node->next = NULL;
        after_node->next = node;
    }
    else {
        node->prev = after_node;
        node->next = after_node->next;
        after_node->next->prev = node;
        after_node->next = node;
    }
}

fpx_err_t
fpx_list_remove_first(fpx_list_t *list, bool (*compar)(const fpx_list_node_t *))
{
    fpx_list_node_t *temp;
    int found;

    temp = list->head;
    found = 0;

    while (temp) {
        if (compar(temp)) {
            found = 1;
            break;
        }
        temp = temp->next;
    }

    if (!found) {
        return FPX_FAILED;
    }

    fpx_list_remove(list, temp);

    return FPX_OK;
}

fpx_err_t
fpx_list_remove_last(fpx_list_t *list, bool (*compar)(const fpx_list_node_t *))
{
    fpx_list_node_t *temp;
    int found;

    temp = list->tail;
    found = 0;

    while (temp) {
        if (compar(temp)) {
            found = 1;
            break;
        }
        temp = temp->prev;
    }

    if (!found) {
        return FPX_FAILED;
    }

    fpx_list_remove(list, temp);

    return FPX_OK;
}

static void
fpx_list_mergesort_merge(fpx_list_t *list, fpx_list_node_t *left,
    fpx_list_node_t *right,
    int (*compar)(const fpx_list_node_t *, const fpx_list_node_t *))
{
    fpx_list_node_t *temp;

    memset(list, 0, sizeof(fpx_list_node_t));

    while (left && right) {
        if (compar(left, right) == -1) {
            temp = left->next;
            fpx_list_push_back(list, left);
            left = temp;
        }
        else {
            temp = right->next;
            fpx_list_push_back(list, right);
            right = temp;
        }
    }

    while (left) {
        temp = left->next;
        fpx_list_push_back(list, left);
        left = temp;
    }

    while (right) {
        temp = right->next;
        fpx_list_push_back(list, right);
        right = temp;
    }
}

void
fpx_list_mergesort(fpx_list_t *list,
    int (*compar)(const fpx_list_node_t *, const fpx_list_node_t *))
{
    fpx_list_node_t *node;
    size_t m, i;

    fpx_list_t list1, list2;

    if (list->size < 2) {
        return;
    }

    m = list->size / 2;
    node = list->head;

    for (i = 1; i < m; ++i) {
        node = node->next;
    }

    list1.head = list->head;
    list1.tail = node;
    list1.size = m;

    list2.head = list1.tail->next;
    list2.tail = list->tail;
    list2.size = list->size - m;

    list1.tail->next = NULL;

    fpx_list_mergesort(&list1, compar);
    fpx_list_mergesort(&list2, compar);

    fpx_list_mergesort_merge(list, list1.head, list2.head, compar);
}
