#include "fpx_test.h"
#include "fpx_portable.h"
#include "fpx_core.h"


fpx_init_stub_head(void *, fpx_malloc, size_t n);
fpx_init_stub_body(fpx_malloc, n);

fpx_init_stub_head(void *, fpx_palloc, fpx_pool_t *pool, size_t n);
fpx_init_stub_body(fpx_palloc, pool, n);

