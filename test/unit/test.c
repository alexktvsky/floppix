#include "fpx_test.h"
#include "fpx_portable.h"
#include "fpx_core.h"

#include "test_fpx_pool.h"


int
main(void)
{
    fpx_run_suite_tests(fpx_pool_tests);

    return 0;
}
