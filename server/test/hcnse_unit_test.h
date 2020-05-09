#ifndef HCNSE_UNIT_TEST_H
#define HCNSE_UNIT_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef void(* hcnse_test_func_t)(void);

typedef struct {
    hcnse_test_func_t func;
    char *func_name;
} hcnse_unit_test_t;


#define hcnse_assert_true(x)                                       \
    if (!(x)) {                                                    \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);          \
        fprintf(stderr,                                            \
            "%s:%d: Assertion 'hcnse_assert_true(%s)' failed.\n",  \
            __FILE__, __LINE__, #x);                               \
        _is_last_test_failed = true;                               \
        return;                                                    \
    }

#define hcnse_assert_int_equal(x, y)                                       \
    if ((x) != (y)) {                                                      \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                  \
        fprintf(stderr,                                                    \
            "%s:%d: Assertion 'hcnse_assert_int_equal(%d, %d)' failed.\n", \
            __FILE__, __LINE__, x, y);                                     \
        _is_last_test_failed = true;                                       \
        return;                                                            \
    }

#define hcnse_assert_ptr_equal(x, y)                                        \
    if (((uintptr_t)x) != ((uintptr_t)y)) {                                 \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                   \
        fprintf(stderr,                                                     \
            "%s:%d: Assertion 'hcnse_assert_ptr_equal(%p, %p)' failed.\n",  \
            __FILE__, __LINE__, x, y);                                      \
        _is_last_test_failed = true;                                        \
        return;                                                             \
    }

#define hcnse_assert_string_equal(x, y)                                       \
    if (strcmp(x, y) != 0) {                                                  \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                     \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_string_equal(%s, %s)' failed.\n", \
            __FILE__, __LINE__, x, y);                                        \
        _is_last_test_failed = true;                                          \
        return;                                                               \
    }

#define hcnse_assert_memory_equal(x, y, n)                                  \
    if (strncmp(x, y, n) != 0) {                                            \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                   \
        fprintf(stderr,                                                     \
            "%s:%d: "                                                       \
            "Assertion 'hcnse_assert_memory_equal(%p, %p, %d)' failed.\n",  \
            __FILE__, __LINE__, x, y, n);                                   \
        _is_last_test_failed = true;                                        \
        return;                                                             \
    }

#define hcnse_assert_non_null(expr)                             \
    if (!(expr)) {                                              \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);       \
        fprintf(stderr, "%s:%d: "                               \
            "Assertion 'hcnse_assert_non_null(%p)' failed.\n",  \
            __FILE__, __LINE__, (void *) expr);                 \
        _is_last_test_failed = true;                            \
        return;                                                 \
    }

#define hcnse_assert_null(expr)                                               \
    if (expr) {                                                               \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                     \
        fprintf(stderr, "%s:%d: Assertion 'hcnse_assert_null(%p)' failed.\n", \
            __FILE__, __LINE__, expr);                                        \
        _is_last_test_failed = true;                                          \
        return;                                                               \
    }


#define hcnse_init_mock(_ret_type, _name, ...)  \
    static _ret_type _name##_return;            \
    _ret_type __wrap_##_name(__VA_ARGS__) {     \
        return _name##_return; }


#define hcnse_will_return(_func, _value) \
    _func##_return = _value


#define hcnse_set_unit_test(_name_unit_test) \
    {_name_unit_test, #_name_unit_test}


#define hcnse_run_unit_tests(...) {                                       \
    hcnse_unit_test_t dummy = {NULL, NULL};                               \
    hcnse_unit_test_t *list = (hcnse_unit_test_t[]){__VA_ARGS__, dummy};  \
    for (size_t i = 0; ; i++) {                                           \
        if (!(list[i]).func) {                                            \
            break;                                                        \
        }                                                                 \
        (list[i]).func();                                                 \
        fprintf(stdout, "[ PASSED ] %s\n", (list[i]).func_name);          \
    }                                                                     \
}


#define hcnse_init_suite_tests(...) {__VA_ARGS__, {NULL, NULL}}
    
extern bool _is_last_test_failed;

void hcnse_run_suite_tests(hcnse_unit_test_t *suite_tests,
    const char *suite_tests_name);

#endif /* HCNSE_UNIT_TEST_H */
