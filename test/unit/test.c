#include "hcnse_test.h"
#include "hcnse_portable.h"
#include "hcnse_core.h"

#include "test_hcnse_pool.h"


int
main(void)
{
    hcnse_run_suite_tests(hcnse_pool_tests);

    return 0;
}
