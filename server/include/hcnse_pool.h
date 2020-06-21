#ifndef INCLUDED_HCNSE_POOL_H
#define INCLUDED_HCNSE_POOL_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_pool_cleanup_add(pool, data, handler) \
    hcnse_pool_cleanup_add1(pool, data, (hcnse_cleanup_handler_t) handler)


hcnse_pool_t *hcnse_pool_create(size_t size, hcnse_pool_t *parent);
hcnse_err_t hcnse_pool_create1(hcnse_pool_t **newpool, size_t size,
    hcnse_pool_t *parent);
void hcnse_pool_add_child(hcnse_pool_t *parent, hcnse_pool_t *new_child);
void *hcnse_palloc(hcnse_pool_t *pool, size_t size);
void *hcnse_pcalloc(hcnse_pool_t *pool, size_t size);
void hcnse_pool_cleanup_add1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler);
void hcnse_pool_clean(hcnse_pool_t *pool);
void hcnse_pool_destroy(hcnse_pool_t *pool);

size_t hcnse_pool_get_size(hcnse_pool_t *pool);
size_t hcnse_pool_get_free_size(hcnse_pool_t *pool);
size_t hcnse_pool_get_total_size(hcnse_pool_t *pool);

#endif /* INCLUDED_HCNSE_POOL_H */
