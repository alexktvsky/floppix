#ifndef HCNSE_UNIT_TEST_H
#define HCNSE_UNIT_TEST_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#define hcnse_define_stub_head(type, name, ...)                               \
    bool __wrap_##name##_do_ret;                                              \
    type __wrap_##name##_ret;                                                 \
    type __real_##name(__VA_ARGS__);                                          \
    type __wrap_##name(__VA_ARGS__) {                                         \
        if (__wrap_##name##_do_ret) {                                         \
            __wrap_##name##_do_ret = false;                                   \
            return __wrap_##name##_ret;

#define hcnse_define_stub_body(name, ...)                                     \
        }                                                                     \
        else {                                                                \
            return __real_##name(__VA_ARGS__);                                \
        }                                                                     \
    }

#define hcnse_declare_stub(type, name, ...)                                   \
    extern bool __wrap_##name##_do_ret;                                       \
    extern type __wrap_##name##_ret;                                          \
    type __real_##name(__VA_ARGS__);                                          \
    type __wrap_##name(__VA_ARGS__);

#define hcnse_stub_will_return(name, val)                                     \
    __wrap_##name##_do_ret = true;                                            \
    __wrap_##name##_ret = val;


#define hcnse_assert_true(exp)                                                \
    if (!(exp)) {                                                             \
        fprintf(stderr, "%s:%d: Assertion hcnse_assert_true(%s) failed\n",    \
            __FILE__, __LINE__, hcnse_stringify(x));                          \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_false(exp)                                               \
    if (exp) {                                                                \
        fprintf(stderr, "%s:%d: Assertion hcnse_assert_true(%s) failed\n",    \
            __FILE__, __LINE__, hcnse_stringify(x));                          \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_int_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_int_equal(%s, %s)' failed\n",     \
            __FILE__, __LINE__, hcnse_stringify(x), hcnse_stringify(y));      \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_size_equal(x, y)                                         \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_size_equal(%s, %s)' failed\n",    \
            __FILE__, __LINE__, hcnse_stringify(x), hcnse_stringify(y));      \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_ptr_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_ptr_equal(%s, %s)' failed\n",     \
            __FILE__, __LINE__, hcnse_stringify(x), hcnse_stringify(y));      \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_str_equal(x, y)                                          \
    if (strcmp(x, y) != 0) {                                                  \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_string_equal(%s, %s)' failed\n",  \
            __FILE__, __LINE__, hcnse_stringify(x), hcnse_stringify(y));      \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_mem_equal(x, y, n)                                       \
    if (memcmp(x, y, n) != 0) {                                               \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_memory_equal(%s, %s, %s)' "       \
            "failed\n", __FILE__, __LINE__, hcnse_stringify(x),               \
            hcnse_stringify(y), hcnse_stringify(n));                          \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_non_null(expr)                                           \
    if ((expr) == NULL) {                                                     \
        fprintf(stderr, "%s:%d: Assertion 'hcnse_assert_non_null(%s)' "       \
            "failed\n", __FILE__, __LINE__, hcnse_stringify(expr));           \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_null(expr)                                               \
    if ((expr) != NULL) {                                                     \
        fprintf(stderr, "%s:%d: Assertion 'hcnse_assert_null(%s)' failed\n",  \
            __FILE__, __LINE__, hcnse_stringify(expr));                       \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_init_suite_tests(...) {__VA_ARGS__, {NULL, NULL}}
#define hcnse_unit_test(name) {name, #name}


#define hcnse_run_suite_tests(tests, suite_name) {                            \
    size_t test_counter = 0;                                                  \
    size_t ok_test_counter = 0;                                               \
    for (size_t i = 0; ; ++i) {                                               \
        if (tests[i].func == NULL) {                                          \
            break;                                                            \
        }                                                                     \
        if (tests[i].func() == HCNSE_OK) {                                    \
            fprintf(stdout, "[ PASSED ] %s\n", tests[i].name);                \
            ok_test_counter += 1;                                             \
        }                                                                     \
        else {                                                                \
            fprintf(stdout, "[ FAILED ] %s\n", tests[i].name);                \
        }                                                                     \
        test_counter += 1;                                                    \
    }                                                                         \
    fprintf(stdout, "Passed %zu of %zu tests in '%s'\n",                      \
        ok_test_counter, test_counter, suite_name);                           \
}

typedef struct {
    hcnse_err_t (*func)(void);
    char *name;
} hcnse_unit_test_t;

#endif /* HCNSE_UNIT_TEST_H */
