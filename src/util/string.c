#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"
#include "floppix/system/pool.h"
#include "floppix/system/memory.h"

int
fpx_atoi(const char *str, size_t n)
{
    int value, temp;
    int cutoff, cutlim;
    unsigned int i;

    if (n == 0) {
        return -1;
    }

    cutoff = FPX_INT_T_MAX / 10;
    cutlim = FPX_INT_T_MAX % 10;

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
fpx_atosz(const char *str, size_t n)
{
    ssize_t value, temp;
    ssize_t cutoff, cutlim;
    unsigned int i;

    if (n == 0) {
        return -1;
    }

    cutoff = FPX_SSIZE_T_MAX / 10;
    cutlim = FPX_SSIZE_T_MAX % 10;

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
fpx_strlen(const char *str)
{
    unsigned int i;

    for (i = 0;; ++i) {

        if (str[i] == '\0') {
            return i;
        }
    }
}

size_t
fpx_strnlen(const char *str, size_t n)
{
    unsigned int i;

    for (i = 0; i < n; ++i) {

        if (str[i] == '\0') {
            return i;
        }
    }

    return n;
}

int
fpx_strcasecmp(char *str1, char *str2)
{
    unsigned int c1, c2;

    for (;;) {
        c1 = (uint) *str1++;
        c2 = (uint) *str2++;

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

int
fpx_strncasecmp(char *str1, char *str2, size_t n)
{
    unsigned int c1, c2;

    while (n) {
        c1 = (uint) *str1++;
        c2 = (uint) *str2++;

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
fpx_strchr(const char *str, int c)
{
    size_t len, i;

    len = fpx_strlen(str);

    for (i = 0; i < len; ++i) {

        if (str[i] == c) {
            return (char *) &str[i];
        }
    }
    return NULL;
}

char *
fpx_strrchr(const char *str, int c)
{
    size_t len;
    int i;

    len = fpx_strlen(str);

    for (i = len; i >= 0; i--) {

        if (str[i] == c) {
            return (char *) &str[i];
        }
    }
    return NULL;
}

char *
fpx_pstrdup(fpx_pool_t *pool, const char *str)
{
    char *mem;
    size_t len;

    len = fpx_strlen(str) + 1;

    mem = fpx_palloc(pool, len * sizeof(char));
    if (!mem) {
        return NULL;
    }

    fpx_memmove(mem, str, len);

    return mem;
}

char *
fpx_pstrndup(fpx_pool_t *pool, const char *str, size_t n)
{
    char *mem;

    mem = fpx_palloc(pool, (n + 1) * sizeof(char));
    if (!mem) {
        return NULL;
    }

    fpx_memmove(mem, str, n);
    mem[n] = '\0';

    return mem;
}

char *
fpx_pstrcat1(fpx_pool_t *pool, ...)
{
    va_list args;
    char *str, *argv, *pos;
    size_t len;

    len = 0;

    va_start(args, pool);

    while ((argv = va_arg(args, char *)) != NULL) {
        len += fpx_strlen(argv);
    }

    va_end(args);

    str = fpx_palloc(pool, len + 1);
    if (!str) {
        return NULL;
    }

    pos = str;

    str[len] = '\0';

    va_start(args, pool);

    while ((argv = va_arg(args, char *)) != NULL) {
        len = fpx_strlen(argv);
        fpx_memmove(pos, argv, len);
        pos += len;
    }

    va_end(args);

    return str;
}
