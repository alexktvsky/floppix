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

    for (i = 0; i < n; i++) {
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

    for (i = 0; i < n; i++) {
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

    for (i = 0; ; i++) {
        if (str[i] == '\0') {
            return i;
        }
    }
}

size_t
hcnse_strnlen(const char *str, size_t n)
{
    hcnse_uint_t i;

    for (i = 0; i < n; i++) {
        if (str[i] == '\0') {
            return i;
        }
    }

    return n;
}
