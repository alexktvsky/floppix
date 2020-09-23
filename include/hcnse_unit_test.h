#ifndef HCNSE_UNIT_TEST_H
#define HCNSE_UNIT_TEST_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#define hcnse_init_stub_head(type, name, ...)                                 \
    bool __wrap_##name##_do_ret;                                              \
    type __wrap_##name##_ret;                                                 \
    type __real_##name(__VA_ARGS__);                                          \
    type __wrap_##name(__VA_ARGS__) {                                         \
        if (__wrap_##name##_do_ret) {                                         \
            __wrap_##name##_do_ret = false;                                   \
            return __wrap_##name##_ret;

#define hcnse_init_stub_body(name, ...)                                       \
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
        fprintf(stderr, "%s:%d: Assertion hcnse_assert_false(%s) failed\n",   \
            __FILE__, __LINE__, hcnse_stringify(x));                          \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_int_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_int_equal(%s, %s)' failed\n"      \
            "expected: " HCNSE_FMT_INT_T "\n"                                 \
            "  actual: " HCNSE_FMT_INT_T "\n",                                \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_uint_equal(x, y)                                         \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_uint_equal(%s, %s)' failed\n"     \
            "expected: " HCNSE_FMT_UINT_T "\n"                                \
            "  actual: " HCNSE_FMT_UINT_T "\n",                               \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_int32_equal(x, y)                                        \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_int32_equal(%s, %s)' failed\n"    \
            "expected: %d\n"                                                  \
            "  actual: %d\n",                                                 \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_uint32_equal(x, y)                                       \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_uint32_equal(%s, %s)' failed\n"   \
            "expected: %u\n"                                                  \
            "  actual: %u\n",                                                 \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_int64_equal(x, y)                                        \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_int64_equal(%s, %s)' failed\n"    \
            "expected: %zd\n"                                                 \
            "  actual: %zd\n",                                                \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_uint64_equal(x, y)                                       \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_uint64_equal(%s, %s)' failed\n"   \
            "expected: %zu\n"                                                 \
            "  actual: %zu\n",                                                \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_size_equal(x, y)                                         \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_size_equal(%s, %s)' failed\n"     \
            "expected: %zu\n"                                                 \
            "  actual: %zu\n",                                                \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_ssize_equal(x, y)                                        \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_ssize_equal(%s, %s)' failed\n"    \
            "expected: %zd\n"                                                 \
            "  actual: %zd\n",                                                \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_ptr_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_ptr_equal(%s, %s)' failed\n"      \
            "expected: %p\n"                                                  \
            "  actual: %p\n",                                                 \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
        return HCNSE_FAILED;                                                  \
    }

#define hcnse_assert_str_equal(x, y)                                          \
    if (strcmp(x, y) != 0) {                                                  \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'hcnse_assert_string_equal(%s, %s)' failed\n"   \
            "expected: %s\n"                                                  \
            "  actual: %s\n",                                                 \
            __FILE__, __LINE__,                                               \
            hcnse_stringify(x), hcnse_stringify(y), x, y);                    \
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

#define hcnse_unit_test(name) {name, #name}
#define hcnse_null_test {NULL, NULL}

#define hcnse_run_suite_tests_by_full_name(suite) {                           \
    size_t test_counter = 0;                                                  \
    size_t failed_test_counter = 0;                                           \
    for (size_t i = 0; ; ++i) {                                               \
        if (suite.tests[i].func == NULL) {                                    \
            break;                                                            \
        }                                                                     \
        if (suite.tests[i].func() == HCNSE_OK) {                              \
            fprintf(stdout, "[ PASSED ] %s\n", suite.tests[i].name);          \
        }                                                                     \
        else {                                                                \
            fprintf(stdout, "[ FAILED ] %s\n", suite.tests[i].name);          \
            failed_test_counter += 1;                                         \
        }                                                                     \
        test_counter += 1;                                                    \
    }                                                                         \
    fprintf(stdout, "%zu test(s), %zu failure(s) in suite '%s'\n",            \
        test_counter, failed_test_counter, suite.name);                       \
}

#define hcnse_run_suite_tests(name) \
    hcnse_run_suite_tests_by_full_name(hcnse_##name##_suite)

#define hcnse_init_suite_tests(name, ...)                                     \
hcnse_unit_test_t hcnse_##name##_suite_tests[] = {                            \
    __VA_ARGS__,                                                              \
    hcnse_null_test                                                           \
};                                                                            \
hcnse_suite_tests_t hcnse_##name##_suite = {                                  \
    hcnse_stringify(name),                                                    \
    hcnse_##name##_suite_tests                                                \
};

#define hcnse_declare_suite_tests(name) \
    extern hcnse_suite_tests_t hcnse_##name##_suite;


typedef struct {
    hcnse_err_t (*func)(void);
    char *name;
} hcnse_unit_test_t;

typedef struct {
    const char *name;
    hcnse_unit_test_t *tests;
} hcnse_suite_tests_t;



#endif /* HCNSE_UNIT_TEST_H */
