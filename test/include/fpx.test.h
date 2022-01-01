#ifndef FPX_TEST_H
#define FPX_TEST_H

#include "fpx.system.os.portable.h"
#include "fpx.system.type.h"
#include "fpx.util.string.h"


#define fpx_init_stub_head(type, name, ...)                                   \
    bool __wrap_##name##_do_ret;                                              \
    type __wrap_##name##_ret;                                                 \
    type __real_##name(__VA_ARGS__);                                          \
    type __wrap_##name(__VA_ARGS__) {                                         \
        if (__wrap_##name##_do_ret) {                                         \
            __wrap_##name##_do_ret = false;                                   \
            return __wrap_##name##_ret;

#define fpx_init_stub_body(name, ...)                                         \
        }                                                                     \
        else {                                                                \
            return __real_##name(__VA_ARGS__);                                \
        }                                                                     \
    }

#define fpx_declare_stub(type, name, ...)                                     \
    extern bool __wrap_##name##_do_ret;                                       \
    extern type __wrap_##name##_ret;                                          \
    type __real_##name(__VA_ARGS__);                                          \
    type __wrap_##name(__VA_ARGS__);

#define fpx_stub_will_return(name, val)                                       \
    __wrap_##name##_do_ret = true;                                            \
    __wrap_##name##_ret = val;


#define fpx_assert_true(exp)                                                  \
    if (!(exp)) {                                                             \
        fprintf(stderr, "%s:%d: Assertion fpx_assert_true(%s) failed\n",      \
            __FILE__, __LINE__, fpx_stringify(x));                            \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_false(exp)                                                 \
    if (exp) {                                                                \
        fprintf(stderr, "%s:%d: Assertion fpx_assert_false(%s) failed\n",     \
            __FILE__, __LINE__, fpx_stringify(x));                            \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_int_equal(x, y)                                            \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_int_equal(%s, %s)' failed\n"        \
            "expected: " FPX_FMT_INT_T "\n"                                   \
            "  actual: " FPX_FMT_INT_T "\n",                                  \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_uint_equal(x, y)                                           \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_uint_equal(%s, %s)' failed\n"       \
            "expected: " FPX_FMT_UINT_T "\n"                                  \
            "  actual: " FPX_FMT_UINT_T "\n",                                 \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_int32_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_int32_equal(%s, %s)' failed\n"      \
            "expected: %d\n"                                                  \
            "  actual: %d\n",                                                 \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_uint32_equal(x, y)                                         \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_uint32_equal(%s, %s)' failed\n"     \
            "expected: %u\n"                                                  \
            "  actual: %u\n",                                                 \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_int64_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_int64_equal(%s, %s)' failed\n"      \
            "expected: %zd\n"                                                 \
            "  actual: %zd\n",                                                \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_uint64_equal(x, y)                                         \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_uint64_equal(%s, %s)' failed\n"     \
            "expected: %zu\n"                                                 \
            "  actual: %zu\n",                                                \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_size_equal(x, y)                                           \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_size_equal(%s, %s)' failed\n"       \
            "expected: %zu\n"                                                 \
            "  actual: %zu\n",                                                \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_ssize_equal(x, y)                                          \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_ssize_equal(%s, %s)' failed\n"      \
            "expected: %zd\n"                                                 \
            "  actual: %zd\n",                                                \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_ptr_equal(x, y)                                            \
    if (x != y) {                                                             \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_ptr_equal(%s, %s)' failed\n"        \
            "expected: %p\n"                                                  \
            "  actual: %p\n",                                                 \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_str_equal(x, y)                                            \
    if (strcmp(x, y) != 0) {                                                  \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_string_equal(%s, %s)' failed\n"     \
            "expected: %s\n"                                                  \
            "  actual: %s\n",                                                 \
            __FILE__, __LINE__,                                               \
            fpx_stringify(x), fpx_stringify(y), x, y);                        \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_mem_equal(x, y, n)                                         \
    if (memcmp(x, y, n) != 0) {                                               \
        fprintf(stderr,                                                       \
            "%s:%d: Assertion 'fpx_assert_memory_equal(%s, %s, %s)' "         \
            "failed\n", __FILE__, __LINE__, fpx_stringify(x),                 \
            fpx_stringify(y), fpx_stringify(n));                              \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_non_null(expr)                                             \
    if ((expr) == NULL) {                                                     \
        fprintf(stderr, "%s:%d: Assertion 'fpx_assert_non_null(%s)' "         \
            "failed\n", __FILE__, __LINE__, fpx_stringify(expr));             \
        return FPX_FAILED;                                                    \
    }

#define fpx_assert_null(expr)                                                 \
    if ((expr) != NULL) {                                                     \
        fprintf(stderr, "%s:%d: Assertion 'fpx_assert_null(%s)' failed\n",    \
            __FILE__, __LINE__, fpx_stringify(expr));                         \
        return FPX_FAILED;                                                    \
    }

#define fpx_unit_test(name) {name, #name}
#define fpx_null_test {NULL, NULL}

#define fpx_run_suite_tests_by_full_name(suite) {                             \
    size_t test_counter = 0;                                                  \
    size_t failed_test_counter = 0;                                           \
    for (size_t i = 0; ; ++i) {                                               \
        if (suite.tests[i].func == NULL) {                                    \
            break;                                                            \
        }                                                                     \
        if (suite.tests[i].func() == FPX_OK) {                                \
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

#define fpx_run_suite_tests(name) \
    fpx_run_suite_tests_by_full_name(fpx_##name##_suite)

#define fpx_init_suite_tests(name, ...)                                       \
fpx_unit_test_t fpx_##name##_suite_tests[] = {                                \
    __VA_ARGS__,                                                              \
    fpx_null_test                                                             \
};                                                                            \
fpx_suite_tests_t fpx_##name##_suite = {                                      \
    fpx_stringify(name),                                                      \
    fpx_##name##_suite_tests                                                  \
};

#define fpx_declare_suite_tests(name) \
    extern fpx_suite_tests_t fpx_##name##_suite;


typedef struct {
    fpx_err_t (*func)(void);
    char *name;
} fpx_unit_test_t;

typedef struct {
    const char *name;
    fpx_unit_test_t *tests;
} fpx_suite_tests_t;



#endif /* FPX_TEST_H */
