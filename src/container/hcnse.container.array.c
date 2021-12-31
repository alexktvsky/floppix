#include "hcnse.container.array.h"
#include "hcnse.system.memory.h"

struct hcnse_array_s {
    void *data;
    size_t size;
    size_t n_total;
    size_t n_current;
};

hcnse_array_t *
hcnse_array_init(hcnse_pool_t *pool, size_t size, size_t n)
{
    hcnse_array_t *array;
    void *data;

    array = hcnse_pcalloc(pool, sizeof(hcnse_array_t));

    data = hcnse_pcalloc(pool, size * n);

    /*
     * Next fields set by hcnse_pcalloc()
     * 
     * array->n_current = 0;
     * 
     */

    array->size = size;
    array->n_total = n;
    array->data = data;

    return array;
}

hcnse_err_t
hcnse_array_push(hcnse_array_t *array, void *data)
{
    void *mem;

    if (array->n_current >= array->n_total) {
        return HCNSE_FAILED;
    }

    mem = ((uint8_t *) array->data) + (array->n_current * array->size);

    hcnse_memmove(mem, data, array->size);

    array->size += 1;

    return HCNSE_OK;
}

void
hcnse_array_clear(hcnse_array_t *array)
{
    array->n_current = 0;
    array->data = NULL;
}
