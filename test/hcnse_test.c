#include "hcnse_unit_test.h"
#include "hcnse_portable.h"
#include "hcnse_core.h"

#include "test_hcnse_pool.h"


int
main(void)
{
    hcnse_unit_test_t tests[] = hcnse_init_suite_tests(
        hcnse_unit_test(test_pool_create_when_malloc_failed),
        hcnse_unit_test(test_a2)
    );
    hcnse_run_suite_tests(tests, "hcnse_pool");


    return 0;
}
