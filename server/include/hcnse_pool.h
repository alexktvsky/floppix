#ifndef INCLUDED_HCNSE_MEMPOOL_H
#define INCLUDED_HCNSE_MEMPOOL_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_pool_cleanup_register(pool, data, handler) \
    hcnse_pool_cleanup_register1(pool, data, (hcnse_cleanup_handler_t) handler)


hcnse_pool_t *hcnse_pool_create(hcnse_pool_t *parent);
hcnse_err_t hcnse_pool_create1(hcnse_pool_t **newpool, hcnse_pool_t *parent);
hcnse_err_t hcnse_pool_add_child(hcnse_pool_t *parent, hcnse_pool_t *child);
void *hcnse_palloc(hcnse_pool_t *pool, size_t in_size);
void *hcnse_pcalloc(hcnse_pool_t *pool, size_t in_size);

void hcnse_pool_run_cleanup(hcnse_pool_t *pool);
void hcnse_pool_cleanup_register1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler);

size_t hcnse_get_pool_size(hcnse_pool_t *pool);
size_t hcnse_get_pool_free_size(hcnse_pool_t *pool);
size_t hcnse_get_pool_total_size(hcnse_pool_t *pool);
void hcnse_pool_clean(hcnse_pool_t *pool);
void hcnse_pool_destroy(hcnse_pool_t *pool);

#endif /* INCLUDED_HCNSE_MEMPOOL_H */
