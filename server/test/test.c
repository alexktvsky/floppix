#include "hcnse_portable.h"
#include "hcnse_core.h"
#include "hcnse_unit_test.h"

#include "test_hcnse_list.h"


int
__wrap_main(void)
{

    hcnse_unit_test_t tests[] = hcnse_init_suite_tests(
        hcnse_set_unit_test(hcnse_list_test_1),
        hcnse_set_unit_test(hcnse_list_test_2)
    );
    hcnse_run_suite_tests(tests, "Generic tests");



    return 0;
}
