#ifndef HCNSE_ARRAY_H
#define HCNSE_ARRAY_H

struct hcnse_array_s {
    void *data;
    size_t size;
    size_t n_total;
    size_t n_current;
};


hcnse_array_t *hcnse_array_create(hcnse_pool_t *pool, size_t size, size_t n);
hcnse_err_t hcnse_array_push(hcnse_array_t *array, void *data);
void hcnse_array_clean(hcnse_array_t *array);


#endif /* HCNSE_ARRAY_H */
