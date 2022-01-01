#ifndef FPX_ALGO_ARRAY_H
#define FPX_ALGO_ARRAY_H

#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"
#include "fpx.system.pool.h"

typedef struct fpx_array_s fpx_array_t;

fpx_array_t *fpx_array_init(fpx_pool_t *pool, fpx_size_t size, fpx_size_t n);
fpx_err_t fpx_array_push(fpx_array_t *array, void *data);
void fpx_array_clear(fpx_array_t *array);

#endif /* FPX_ALGO_ARRAY_H */
