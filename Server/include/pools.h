#ifndef POOLS_H
#define POOLS_H

typedef struct pool_s pool_t;

status_t pool_create(pool_t **newpool, pool_t *parent);
void *palloc(pool_t *pool, size_t in_size);
void *pcalloc(pool_t *pool, size_t in_size);
size_t get_pool_size(pool_t *pool);
size_t get_pool_free_size(pool_t *pool);
size_t get_pool_total_size(pool_t *pool);
void pool_clear(pool_t *pool);
void pool_destroy(pool_t *pool);

#endif /* POOLS_H */
