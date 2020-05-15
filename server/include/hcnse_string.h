#ifndef INCLUDED_HCNSE_STRING_H
#define INCLUDED_HCNSE_STRING_H

#define HCNSE_LF                       "\n"
#define HCNSE_CR                       "\r"
#define HCNSE_CRLF                     "\r\n"

#if (HCNSE_POSIX)
#define HCNSE_PORTABLE_LF              HCNSE_LF
#elif (HCNSE_WIN32)
#define HCNSE_PORTABLE_LF              HCNSE_CRLF
#endif

#define HCNSE_METRIC_PREFIX_EMPTY      ""
#define HCNSE_METRIC_PREFIX_KILO       "k"
#define HCNSE_METRIC_PREFIX_MEGA       "m"
#define HCNSE_METRIC_PREFIX_GIGA       "g"
#define HCNSE_METRIC_PREFIX_TERA       "t"

#define HCNSE_METRIC_MULTIPLIER_KILO   1000
#define HCNSE_METRIC_MULTIPLIER_MEGA   1000000
#define HCNSE_METRIC_MULTIPLIER_GIGA   1000000000
#define HCNSE_METRIC_MULTIPLIER_TERA   1000000000000


#define hcnse_strlen(str)              strlen(str)
#define hcnse_memset(buf, c, n)        memset(buf, c, n)
#define hcnse_memmove(dst, src, n)     memmove(dst, src, n)
#define hcnse_memcmp(s1, s2, n)        memcmp(s1, s2, n)
#define hcnse_strcmp(s1, s2, n)        strcmp(s1, s2, n)
#define hcnse_strncmp(s1, s2, n)       strncmp(s1, s2, n)
#define hcnse_atoi(str)                atoi(str)
#define hcnse_snprintf(str, size, ...) \
    snprintf(str, size, __VA_ARGS__)
#define hcnse_vsnprintf(str, size, ...) \
    vsnprintf(str, size, __VA_ARGS__)


const char *hcnse_get_metric_prefix(size_t number);
size_t hcnse_convert_to_prefix(size_t number);

#endif /* INCLUDED_HCNSE_STRING_H */
