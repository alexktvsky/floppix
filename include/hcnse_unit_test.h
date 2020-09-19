#ifndef HCNSE_UNIT_TEST_H
#define HCNSE_UNIT_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define hcnse_helper1(x)  
#define hcnse_expr_to_str(x)     hcnse_helper1(x)

#define hcnse_declare_mock_head(type, name, ...)                               \
    int __wrap_##name##_do_ret;                                                \
    type __wrap_##name##_ret;                                                  \
    type __real_##name(__VA_ARGS__);                                           \
    type __wrap_##name(__VA_ARGS__) {                                          \
        if (__wrap_##name##_do_ret) {                                          \
            __wrap_##name##_do_ret = 0;                                        \
            return __wrap_##name##_ret;

#define hcnse_declare_mock_body(name, ...)                                     \
        }                                                                      \
        else {                                                                 \
            return __real_##name(__VA_ARGS__);                                 \
        }                                                                      \
    }

#define hcnse_will_return(name, val)                                           \
    __wrap_##name##_do_ret = 1;                                                \
    __wrap_##name##_ret = val;


#define hcnse_assert_true(exp)                                                 \
    if (!(exp)) {                                                              \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr, "%s:%d: Assertion hcnse_assert_true(%s) failed\n",     \
            __FILE__, __LINE__, hcnse_expr_to_str(x));                         \
        return;                                                                \
    }

#define hcnse_assert_int_equal(x, y)                                           \
    if (x != y) {                                                              \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr,                                                        \
            "%s:%d: Assertion 'hcnse_assert_int_equal(%s, %s)' failed\n",      \
            __FILE__, __LINE__, hcnse_expr_to_str(x), hcnse_expr_to_str(y));   \
        return;                                                                \
    }

#define hcnse_assert_ptr_equal(x, y)                                           \
    if (x != y) {                                                              \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr,                                                        \
            "%s:%d: Assertion 'hcnse_assert_ptr_equal(%s, %s)' failed\n",      \
            __FILE__, __LINE__, hcnse_expr_to_str(x), hcnse_expr_to_str(y));   \
        return;                                                                \
    }

#define hcnse_assert_string_equal(x, y)                                        \
    if (strcmp(x, y) != 0) {                                                   \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr,                                                        \
            "%s:%d: Assertion 'hcnse_assert_string_equal(%s, %s)' failed\n",   \
            __FILE__, __LINE__, hcnse_expr_to_str(x), hcnse_expr_to_str(y));   \
        return;                                                                \
    }

#define hcnse_assert_memory_equal(x, y, n)                                     \
    if (memcmp(x, y, n) != 0) {                                                \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr,                                                        \
            "%s:%d: Assertion 'hcnse_assert_memory_equal(%s, %s, %s)' "        \
            "failed\n", __FILE__, __LINE__, hcnse_expr_to_str(x),              \
            hcnse_expr_to_str(y), hcnse_expr_to_str(n));                       \
        return;                                                                \
    }

#define hcnse_assert_non_null(expr)                                            \
    if ((expr) == NULL) {                                                      \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr, "%s:%d: Assertion 'hcnse_assert_non_null(%s)' "        \
            "failed.\n", __FILE__, __LINE__, hcnse_expr_to_str(expr));         \
        return;                                                                \
    }

#define hcnse_assert_null(expr)                                                \
    if ((expr) != NULL) {                                                      \
        fprintf(stdout, "[ FAILED ] %s\n", __FUNCTION__);                      \
        fprintf(stderr, "%s:%d: Assertion 'hcnse_assert_null(%s)' failed.\n",  \
            __FILE__, __LINE__, hcnse_expr_to_str(expr));                      \
        return;                                                                \
    }

#endif /* HCNSE_UNIT_TEST_H */
