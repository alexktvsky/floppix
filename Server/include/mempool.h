#ifndef MEMPOOL_H
#define MEMPOOL_H

typedef struct mempool_s mempool_t;

status_t mempool_create(mempool_t **newpool, mempool_t *parent);
void *palloc(mempool_t *pool, size_t in_size);
void *pcalloc(mempool_t *pool, size_t in_size);
size_t get_mempool_size(mempool_t *pool);
size_t get_mempool_free_size(mempool_t *pool);
size_t get_mempool_total_size(mempool_t *pool);
void mempool_clear(mempool_t *pool);
void mempool_destroy(mempool_t *pool);

#endif /* MEMPOOL_H */
