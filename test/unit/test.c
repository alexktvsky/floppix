#include "fpx.test.h"
#include "fpx.test.unit.stubs.h"

#include "fpx.test.unit.pool.h"


int
main(void)
{
    fpx_run_suite_tests(fpx_pool_tests);

    return 0;
}
