#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/TestDB.h>

#include "connect_tests.h"


int main(void)
{
	if (CU_initialize_registry() != CUE_SUCCESS) {
		return CU_get_error();
	}

	CU_TestInfo test_array1[] = {
		{"init_connect_tcp_test1", init_connect_tcp_test1},
		{"init_connect_tcp_test2", init_connect_tcp_test2},
		{"init_connect_tcp_test3", init_connect_tcp_test3},
		{"init_connect_tcp_test4", init_connect_tcp_test4},
		CU_TEST_INFO_NULL,
	};

	CU_SuiteInfo suites[] = {
		{"init_connect.c", NULL, NULL, NULL, NULL, test_array1},
		CU_SUITE_INFO_NULL,
	};


	CU_ErrorCode error = CU_register_suites (suites);
	if (error) {
		return error;
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
