#ifndef HCNSE_CONTAINER_ARRAY_H
#define HCNSE_CONTAINER_ARRAY_H

#include "hcnse.system.os.portable.h"
#include "hcnse.system.errno.h"
#include "hcnse.system.pool.h"

typedef struct hcnse_array_s hcnse_array_t;

hcnse_array_t *hcnse_array_create(hcnse_pool_t *pool, size_t size, size_t n);
hcnse_err_t hcnse_array_push(hcnse_array_t *array, void *data);
void hcnse_array_clear(hcnse_array_t *array);

#endif /* HCNSE_CONTAINER_ARRAY_H */
