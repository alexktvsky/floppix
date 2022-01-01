#include "fpx_portable.h"
#include "fpx_core.h"
#include "fpx_test.h"
#include "fpx_stubs.h"


fpx_err_t
test_pool_create_when_malloc_failed(void)
{
    fpx_pool_t *pool;

    fpx_set_errno(FPX_FAILED);
    fpx_stub_will_return(fpx_malloc, NULL);

    pool = fpx_pool_create(0, NULL);
    fpx_assert_null(pool);

    return FPX_OK;
}

fpx_err_t
test_a2(void)
{
    fpx_assert_int32_equal(1, 1);
    return FPX_OK;
}

fpx_init_suite_tests(fpx_pool_tests,
    fpx_unit_test(test_pool_create_when_malloc_failed),
    fpx_unit_test(test_a2)
);
