#ifndef INCLUDED_MEMPOOL_H
#define INCLUDED_MEMPOOL_H

#include <stddef.h>

#include "server/errors.h"


typedef struct hcnse_mempool_s hcnse_mempool_t;

hcnse_err_t hcnse_mempool_create(hcnse_mempool_t **newpool, hcnse_mempool_t *parent);
void *hcnse_palloc(hcnse_mempool_t *pool, size_t in_size);
void *hcnse_pcalloc(hcnse_mempool_t *pool, size_t in_size);
size_t hcnse_get_mempool_size(hcnse_mempool_t *pool);
size_t hcnse_get_mempool_free_size(hcnse_mempool_t *pool);
size_t hcnse_get_mempool_total_size(hcnse_mempool_t *pool);
void hcnse_mempool_clear(hcnse_mempool_t *pool);
void hcnse_mempool_destroy(hcnse_mempool_t *pool);

#endif /* INCLUDED_MEMPOOL_H */
