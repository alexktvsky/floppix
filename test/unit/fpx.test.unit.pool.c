#include "fpx.test.unit.pool.h"


fpx_err_t
test_pool_create_when_malloc_failed(void)
{
    fpx_pool_t *pool;

    fpx_set_errno(FPX_FAILED);
    fpx_stub_will_return(fpx_malloc1, NULL);

    fpx_assert_true(fpx_pool_create(&pool, 0, NULL) != FPX_OK);

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
