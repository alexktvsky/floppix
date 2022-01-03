#ifndef FPX_UTIL_STRING_H
#define FPX_UTIL_STRING_H

#include "fpx.system.type.h"
#include "fpx.system.pool.h"

#define FPX_NULL     0x0
#define FPX_TAB      0x9
#define FPX_LF       0xA
#define FPX_CR       0xD
#define FPX_SPACE    0x20

#define FPX_LF_STR   "\n"
#define FPX_CR_STR   "\r"
#define FPX_CRLF_STR "\r\n"

#if (FPX_POSIX)
#define FPX_EOL_STR FPX_LF_STR
#elif (FPX_WIN32)
#define FPX_EOL_STR FPX_CRLF_STR
#endif

#define fpx_strcmp(s1, s2)            strcmp(s1, s2)
#define fpx_strncmp(s1, s2, n)        strncmp(s1, s2, n)
#define fpx_snprintf(str, size, ...)  snprintf(str, size, __VA_ARGS__)
#define fpx_vsnprintf(str, size, ...) vsnprintf(str, size, __VA_ARGS__)

#define fpx_isdigit(c)                ((c >= '0') && (c <= '9'))

#define fpx_isalpha(c)                                                         \
    (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))

#define fpx_isascii(c)      ((((int8_t) c) >= 0x0) && (((uint8_t) c) <= 0x7f))

#define fpx_tolower(c)      ((char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c))

#define fpx_toupper(c)      ((char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c))

#define fpx_value_helper(x) #x
#define fpx_value(x)        fpx_value_helper(x)
#define fpx_stringify(x)    fpx_value_helper(x)

#define fpx_pstrcat(...)    fpx_pstrcat1(__VA_ARGS__, NULL)

fpx_int_t fpx_atoi(const char *str, fpx_size_t n);
fpx_ssize_t fpx_atosz(const char *str, fpx_size_t n);
fpx_size_t fpx_strlen(const char *str);
fpx_size_t fpx_strnlen(const char *str, fpx_size_t n);
fpx_int_t fpx_strcasecmp(char *str1, char *str2);
fpx_int_t fpx_strncasecmp(char *str1, char *str2, fpx_size_t n);
char *fpx_strchr(const char *str, fpx_int_t c);
char *fpx_strrchr(const char *str, fpx_int_t c);
char *fpx_pstrdup(fpx_pool_t *pool, const char *str);
char *fpx_pstrndup(fpx_pool_t *pool, const char *str, fpx_size_t n);
char *fpx_pstrcat1(fpx_pool_t *pool, ...);

#endif /* FPX_UTIL_STRING_H */
