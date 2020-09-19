#include "hcnse_portable.h"
#include "hcnse_core.h"
#include "hcnse_unit_test.h"
#include "hcnse_stubs.h"


hcnse_err_t
test_pool_create_when_malloc_failed(void)
{
    hcnse_pool_t *pool;

    hcnse_set_errno(HCNSE_FAILED);
    hcnse_stub_will_return(hcnse_malloc, NULL);

    pool = hcnse_pool_create(0, NULL);
    hcnse_assert_null(pool);

    return HCNSE_OK;
}

hcnse_err_t
test_a2(void)
{

    return HCNSE_OK;
}

