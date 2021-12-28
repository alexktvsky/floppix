#ifndef HCNSE_SYSTEM_POOL_H
#define HCNSE_SYSTEM_POOL_H

#include "hcnse.system.errno.h"

#define hcnse_pool_cleanup_add(pool, data, handler) \
    hcnse_pool_cleanup_add1(pool, data, (hcnse_cleanup_handler_t) handler)

#define hcnse_pool_cleanup_run(pool, data, handler) \
    hcnse_pool_cleanup_run1(pool, data, (hcnse_cleanup_handler_t) handler)

#define hcnse_pool_cleanup_remove(pool, data, handler) \
    hcnse_pool_cleanup_remove1(pool, data, (hcnse_cleanup_handler_t) handler)

typedef struct hcnse_pool_s hcnse_pool_t;
typedef void (*hcnse_cleanup_handler_t)(void *data);

hcnse_pool_t *hcnse_pool_create(size_t size, hcnse_pool_t *parent);
hcnse_err_t hcnse_pool_create1(hcnse_pool_t **newpool, size_t size,
    hcnse_pool_t *parent);
void hcnse_pool_add_child(hcnse_pool_t *parent, hcnse_pool_t *new_child);
void *hcnse_palloc(hcnse_pool_t *pool, size_t size);
void *hcnse_pcalloc(hcnse_pool_t *pool, size_t size);
void hcnse_pool_cleanup_add1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler);
void hcnse_pool_cleanup_run1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler);
void hcnse_pool_cleanup_remove1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler);
void hcnse_pool_clear(hcnse_pool_t *pool);
void hcnse_pool_destroy(hcnse_pool_t *pool);

size_t hcnse_pool_get_size(hcnse_pool_t *pool);
size_t hcnse_pool_get_free_size(hcnse_pool_t *pool);
size_t hcnse_pool_get_total_size(hcnse_pool_t *pool);

#endif /* HCNSE_SYSTEM_POOL_H */
