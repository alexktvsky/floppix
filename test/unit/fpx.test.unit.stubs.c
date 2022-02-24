#include "fpx.test.h"


fpx_init_stub_head(void *, fpx_malloc1, const char *filename, int line,
    size_t n);
fpx_init_stub_body(fpx_malloc1, filename, line, n);

fpx_init_stub_head(void *, fpx_palloc1, const char *filename, int line,
    fpx_pool_t *pool, size_t n);
fpx_init_stub_body(fpx_palloc1, filename, line, pool, n);
