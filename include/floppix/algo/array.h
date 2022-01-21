#ifndef FLOPPIX_ALGO_ARRAY_H
#define FLOPPIX_ALGO_ARRAY_H

#include "floppix/system/os/portable.h"
#include "floppix/system/errno.h"
#include "floppix/system/pool.h"

typedef struct fpx_array_s fpx_array_t;

fpx_array_t *fpx_array_init(fpx_pool_t *pool, fpx_size_t size, fpx_size_t n);
fpx_err_t fpx_array_push(fpx_array_t *array, void *data);
void fpx_array_clear(fpx_array_t *array);

#endif /* FLOPPIX_ALGO_ARRAY_H */
