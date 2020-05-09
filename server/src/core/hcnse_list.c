#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_list_cast_ptr(node) \
    (((void *) node) + sizeof(hcnse_listnode_t))

#define hcnse_list_cast_node_by_ptr(addr) \
    ((hcnse_listnode_t *) (((void *) addr) - sizeof(hcnse_listnode_t)))


typedef struct hcnse_listnode_s hcnse_listnode_t;

struct hcnse_list_s {
    size_t size;
    hcnse_listnode_t *head;
    hcnse_listnode_t *tail;
};

struct hcnse_listnode_s {
    hcnse_listnode_t *next;
    hcnse_listnode_t *prev;
};

size_t hcnse_listnode_t_size = sizeof(hcnse_listnode_t);
size_t hcnse_list_t_size = sizeof(hcnse_list_t);

hcnse_list_t *
hcnse_list_create(void)
{
    hcnse_list_t *new_list = hcnse_malloc(sizeof(hcnse_list_t));
    if (!new_list) {
        return NULL;
    }
    hcnse_memset(new_list, 0, sizeof(hcnse_list_t));
    return new_list;
}

hcnse_err_t
hcnse_list_create1(hcnse_list_t **list)
{
    hcnse_list_t *new_list = hcnse_malloc(sizeof(hcnse_list_t));
    if (!new_list) {
        return hcnse_get_errno();
    }
    hcnse_memset(new_list, 0, sizeof(hcnse_list_t));
    *list = new_list;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_init(hcnse_list_t *list)
{
    hcnse_memset(list, 0, sizeof(hcnse_list_t));
    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_append(hcnse_list_t *list, void *ptr)
{
    hcnse_listnode_t *node = hcnse_list_cast_node_by_ptr(ptr);
    /* If list is empty */
    if (!list->head) {
        list->head = node;
        node->prev = NULL;
    }
    else {
        list->tail->next = node;
        node->prev = list->tail;
    }
    list->tail = node;

    node->next = NULL;
    list->size += 1;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_list_remove(hcnse_list_t *list, void *ptr)
{
    hcnse_listnode_t *temp1 = list->head;
    hcnse_listnode_t *node = hcnse_list_cast_node_by_ptr(ptr);

    /* If first element */
    if (list->head == node) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        // hcnse_free(temp1);
        list->size -= 1;
        return HCNSE_OK;
    }
    /* If the last element */
    else if (list->tail == node) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        // hcnse_free(temp2);
        list->size -= 1;
        return HCNSE_OK;
    }
    else {
        temp1 = temp1->next;
        while (temp1 != node) {
            temp1 = temp1->next;
            if (temp1 == list->tail) {
                return HCNSE_FAILED;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        // hcnse_free(temp1);
        list->size -= 1;
        return HCNSE_OK;
    }
}

void *
hcnse_list_first(hcnse_list_t *list)
{
    if (list->head) {
        return hcnse_list_cast_ptr(list->head);
    }
    else {
        return NULL;
    }
}

void *
hcnse_list_last(hcnse_list_t *list)
{
    if (list->tail) {
        return hcnse_list_cast_ptr(list->tail);
    }
    else {
        return NULL;
    }
}

void *
hcnse_list_next(void *ptr)
{
    hcnse_listnode_t *node;
    node = hcnse_list_cast_node_by_ptr(ptr);
    if (node->next) {
        return hcnse_list_cast_ptr(node->next);
    }
    else {
        return NULL;
    }
}

void *
hcnse_list_prev(void *ptr)
{
    hcnse_listnode_t *node;
    node = hcnse_list_cast_node_by_ptr(ptr);
    if (node->prev) {
        return hcnse_list_cast_ptr(node->prev);
    }
    else {
        return NULL;
    }
}

size_t
hcnse_list_size(hcnse_list_t *list)
{
    return list->size;
}

void
hcnse_list_clean(hcnse_list_t *list)
{
    hcnse_memset(list, 0, sizeof(hcnse_list_t));
}

void
hcnse_list_destroy(hcnse_list_t *list)
{
    hcnse_free(list);
}

void *
hcnse_list_init_node(void *mem)
{
    void *node = mem;
    ((hcnse_listnode_t *) node)->next = NULL;
    ((hcnse_listnode_t *) node)->prev = NULL;
    return hcnse_list_cast_ptr(node);
}

void *
hcnse_list_create_node(size_t size)
{
    void *node = hcnse_malloc(sizeof(hcnse_listnode_t) + size);
    if (!node) {
        return NULL;
    }
    ((hcnse_listnode_t *) node)->next = NULL;
    ((hcnse_listnode_t *) node)->prev = NULL;
    return hcnse_list_cast_ptr(node);
}

hcnse_err_t
hcnse_list_create_node1(void **ptr, size_t size)
{
    hcnse_listnode_t *node = hcnse_malloc(sizeof(hcnse_listnode_t) + size);
    if (!node) {
        return hcnse_get_errno();
    }
    ((hcnse_listnode_t *) node)->next = NULL;
    ((hcnse_listnode_t *) node)->prev = NULL;
    *ptr = hcnse_list_cast_ptr(node);
    return HCNSE_OK;
}

void
hcnse_list_destroy_node(void *ptr)
{
    hcnse_free(hcnse_list_cast_node_by_ptr(ptr));
}

void *
hcnse_list_init_node_and_append(void *mem, hcnse_list_t *list)
{
    void *ptr = mem;
    if (hcnse_list_append(list, ptr) != HCNSE_OK) {
        hcnse_list_destroy_node(ptr);
        return NULL;
    }
    return ptr;
}

void *
hcnse_list_create_node_and_append(size_t size, hcnse_list_t *list)
{
    void *ptr = hcnse_list_create_node(size);
    if (!ptr) {
        return NULL;
    }
    if (hcnse_list_append(list, ptr) != HCNSE_OK) {
        hcnse_list_destroy_node(ptr);
        return NULL;
    }
    return ptr;
}

void
hcnse_list_remove_and_destroy_node(hcnse_list_t *list, void *ptr)
{
    hcnse_list_remove(list, ptr);
    hcnse_list_destroy_node(ptr);
    return;
}

void *
hcnse_try_use_already_exist_node(size_t size, hcnse_list_t *free_nodes,
    hcnse_list_t *list)
{
    void *ptr;

    /* Check for empty already exist node */
    ptr = hcnse_list_first(free_nodes);

    /* Use already exist node */
    if (ptr) {
        hcnse_list_remove(free_nodes, ptr);
        hcnse_list_append(list, ptr);
    }
    /* If there are not empty nodes, create new */
    else {
        ptr = hcnse_list_create_node(size);
        if (!ptr) {
            goto failed;
        }
        hcnse_list_append(list, ptr);
    }

    return ptr;

failed:
    return NULL;
}

void *
hcnse_try_use_already_exist_node1(void *mem, hcnse_list_t *free_nodes,
    hcnse_list_t *list)
{
    void *ptr;

    /* Check for empty already exist node */
    ptr = hcnse_list_first(free_nodes);

    /* Use already exist node */
    if (ptr) {
        hcnse_list_remove(free_nodes, ptr);
        hcnse_list_append(list, ptr);
    }
    /* If there are not empty nodes, allocate in buf */
    else {
        ptr = hcnse_list_cast_ptr(mem);
        if (hcnse_list_append(list, ptr) != HCNSE_OK) {
            goto failed;
        }
    }

    return ptr;

failed:
    return NULL;
}

void
hcnse_list_reserve_node(void *instance, hcnse_list_t *free_nodes,
    hcnse_list_t *list)
{
    void *ptr;
    ptr = hcnse_list_cast_node_by_ptr(instance);
    hcnse_list_remove(list, ptr);
    hcnse_list_append(free_nodes, ptr);
    return;
}
