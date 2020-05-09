#include "hcnse_unit_test.h"


bool _is_last_test_failed;


void hcnse_run_suite_tests(hcnse_unit_test_t *suite_tests,
    const char *suite_tests_name)
{
    size_t test_counter = 0;
    size_t ok_test_counter = 0;
    fprintf(stdout, "Run '%s'...\n", suite_tests_name);
    hcnse_unit_test_t *list = suite_tests;
    for (size_t i = 0; ; i++) {
        if (!(list[i]).func) {
            break;
        }
        (list[i]).func();
        if (!_is_last_test_failed) {
            fprintf(stdout, "[ PASSED ] %s\n", (list[i]).func_name);
            ok_test_counter += 1;
        }
        else {
            _is_last_test_failed = false;
        }
        test_counter += 1;
    }
    fprintf(stdout, "Passed %zu of %zu tests in '%s'\n",
                    ok_test_counter, test_counter, suite_tests_name);
}
