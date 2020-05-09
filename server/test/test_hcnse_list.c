#include "hcnse_portable.h"
#include "hcnse_core.h"
#include "hcnse_unit_test.h"


void hcnse_list_test_1(void)
{
    hcnse_list_t *list = hcnse_list_create();
    hcnse_assert_non_null(list);
}

void hcnse_list_test_2(void)
{
    hcnse_assert_true(1 == 1);
    hcnse_assert_int_equal(5, 5);
    hcnse_assert_ptr_equal(NULL, NULL);
    hcnse_assert_string_equal("hello", "hello");
    hcnse_assert_memory_equal("hello", "hello", 5);
    hcnse_assert_non_null(0xff);
    hcnse_assert_null(NULL);
}

