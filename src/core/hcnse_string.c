#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_int_t
hcnse_atoi(const char *str, size_t n)
{
    hcnse_int_t value, temp;
    hcnse_int_t cutoff, cutlim;
    hcnse_uint_t i;

    if (n == 0) {
        return -1;
    }

    cutoff = HCNSE_INT_T_MAX / 10;
    cutlim = HCNSE_INT_T_MAX % 10;

    value = 0;

    for (i = 0; i < n; ++i) {
        if (str[i] < '0' || str[i] > '9') {
            return -1;
        }

        temp = str[i] - '0';

        if (value >= cutoff && (value > cutoff || temp > cutlim)) {
            return -1;
        }

        value = value * 10 + temp;
    }

    return value;
}

ssize_t
hcnse_atosz(const char *str, size_t n)
{
    ssize_t value, temp;
    ssize_t cutoff, cutlim;
    hcnse_uint_t i;

    if (n == 0) {
        return -1;
    }

    cutoff = HCNSE_SSIZE_T_MAX / 10;
    cutlim = HCNSE_SSIZE_T_MAX % 10;

    value = 0;

    for (i = 0; i < n; ++i) {
        if (str[i] < '0' || str[i] > '9') {
            return -1;
        }

        temp = str[i] - '0';

        if (value >= cutoff && (value > cutoff || temp > cutlim)) {
            return -1;
        }

        value = value * 10 + temp;
    }

    return value;
}

size_t
hcnse_strlen(const char *str)
{
    hcnse_uint_t i;

    for (i = 0; ; ++i) {

        if (str[i] == '\0') {
            return i;
        }
    }
}

size_t
hcnse_strnlen(const char *str, size_t n)
{
    hcnse_uint_t i;

    for (i = 0; i < n; ++i) {

        if (str[i] == '\0') {
            return i;
        }
    }

    return n;
}

hcnse_int_t
hcnse_strcasecmp(char *str1, char *str2)
{
    hcnse_uint_t c1, c2;

    for ( ; ; ) {
        c1 = (hcnse_uint_t) *str1++;
        c2 = (hcnse_uint_t) *str2++;

        c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
        c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

        if (c1 == c2) {

            if (c1) {
                continue;
            }

            return 0;
        }

        return c1 - c2;
    }
}

hcnse_int_t
hcnse_strncasecmp(char *str1, char *str2, size_t n)
{
    hcnse_uint_t c1, c2;

    while (n) {
        c1 = (hcnse_uint_t) *str1++;
        c2 = (hcnse_uint_t) *str2++;

        c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
        c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

        if (c1 == c2) {

            if (c1) {
                n--;
                continue;
            }

            return 0;
        }

        return c1 - c2;
    }

    return 0;
}

char *
hcnse_strchr(const char *str, int c)
{
    size_t len, i;

    len = hcnse_strlen(str);

    for (i = 0; i < len; ++i) {

        if (str[i] == c) {
            return (char *) &str[i];
        }
    }
    return NULL;
}

char *
hcnse_strrchr(const char *str, int c)
{
    size_t len;
    hcnse_int_t i;

    len = hcnse_strlen(str);

    for (i = len; i >= 0; i--) {

        if (str[i] == c) {
            return (char *) &str[i];
        }
    }
    return NULL;
}

char *
hcnse_pstrdup(hcnse_pool_t *pool, const char *str)
{
    char *mem;
    size_t len;

    len = hcnse_strlen(str) + 1;

    mem = hcnse_palloc(pool, len * sizeof(char));
    if (!mem) {
        return NULL;
    }

    hcnse_memmove(mem, str, len);

    return mem;
}

char *
hcnse_pstrndup(hcnse_pool_t *pool, const char *str, size_t n)
{
    char *mem;

    mem = hcnse_palloc(pool, (n + 1) * sizeof(char));
    if (!mem) {
        return NULL;
    }

    hcnse_memmove(mem, str, n);
    mem[n] = '\0';

    return mem;
}

char *
hcnse_pstrcat1(hcnse_pool_t *pool, ...)
{
    va_list args;
    char *str, *argv, *pos;
    size_t len;

    len = 0;

    va_start(args, pool);

    while ((argv = va_arg(args, char *)) != NULL) {
        len += hcnse_strlen(argv);
    }

    va_end(args);

    str = hcnse_palloc(pool, len + 1);
    if (!str) {
        return NULL;
    }

    pos = str;

    str[len] = '\0';

    va_start(args, pool);

    while ((argv = va_arg(args, char *)) != NULL) {
        len = hcnse_strlen(argv);
        hcnse_memmove(pos, argv, len);
        pos += len;
    }

    va_end(args);

    return str;
}
