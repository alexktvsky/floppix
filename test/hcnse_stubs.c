#include "hcnse_unit_test.h"
#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_define_stub_head(void *, hcnse_malloc, size_t n);
hcnse_define_stub_body(hcnse_malloc, n);

hcnse_define_stub_head(void *, hcnse_palloc, hcnse_pool_t *pool, size_t n);
hcnse_define_stub_body(hcnse_palloc, pool, n);

