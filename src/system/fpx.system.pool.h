#ifndef FPX_SYSTEM_POOL_H
#define FPX_SYSTEM_POOL_H

#include "fpx.system.errno.h"

#define fpx_pool_cleanup_add(pool, data, handler) \
    fpx_pool_cleanup_add1(pool, data, (fpx_cleanup_handler_t) handler)

#define fpx_pool_cleanup_run(pool, data, handler) \
    fpx_pool_cleanup_run1(pool, data, (fpx_cleanup_handler_t) handler)

#define fpx_pool_cleanup_remove(pool, data, handler) \
    fpx_pool_cleanup_remove1(pool, data, (fpx_cleanup_handler_t) handler)

typedef struct fpx_pool_s fpx_pool_t;
typedef void (*fpx_cleanup_handler_t)(void *data);

fpx_err_t fpx_pool_create(fpx_pool_t **newpool, size_t size,
    fpx_pool_t *parent);
void fpx_pool_add_child(fpx_pool_t *parent, fpx_pool_t *new_child);
void *fpx_palloc(fpx_pool_t *pool, size_t size);
void *fpx_pcalloc(fpx_pool_t *pool, size_t size);
void fpx_pool_cleanup_add1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler);
void fpx_pool_cleanup_run1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler);
void fpx_pool_cleanup_remove1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler);
void fpx_pool_clear(fpx_pool_t *pool);
void fpx_pool_destroy(fpx_pool_t *pool);

size_t fpx_pool_get_size(fpx_pool_t *pool);
size_t fpx_pool_get_free_size(fpx_pool_t *pool);
size_t fpx_pool_get_total_size(fpx_pool_t *pool);

#endif /* FPX_SYSTEM_POOL_H */
