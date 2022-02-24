#include "floppix/algo/array.h"
#include "floppix/system/memory.h"

struct fpx_array_s {
    void *data;
    size_t size;
    size_t n_total;
    size_t n_current;
};

fpx_array_t *
fpx_array_init(fpx_pool_t *pool, size_t size, size_t n)
{
    fpx_array_t *array;
    void *data;

    array = fpx_pcalloc(pool, sizeof(fpx_array_t));

    data = fpx_pcalloc(pool, size * n);

    /*
     * Next fields set by fpx_pcalloc()
     *
     * array->n_current = 0;
     *
     */

    array->size = size;
    array->n_total = n;
    array->data = data;

    return array;
}

fpx_err_t
fpx_array_push(fpx_array_t *array, void *data)
{
    void *mem;

    if (array->n_current >= array->n_total) {
        return FPX_FAILED;
    }

    mem = ((uint8_t *) array->data) + (array->n_current * array->size);

    fpx_memmove(mem, data, array->size);

    array->size += 1;

    return FPX_OK;
}

void
fpx_array_clear(fpx_array_t *array)
{
    array->n_current = 0;
    array->data = NULL;
}
