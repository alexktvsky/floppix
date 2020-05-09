#ifndef INCLUDED_HCNSE_STRING_H
#define INCLUDED_HCNSE_STRING_H

#define HCNSE_LF                       "\012"
#define HCNSE_CR                       "\015"
#define HCNSE_CRLF                     "\015\012"

#if (HCNSE_POSIX)
#define HCNSE_PORTABLE_LF              HCNSE_LF
#elif (HCNSE_WIN32)
#define HCNSE_PORTABLE_LF              HCNSE_CRLF
#endif

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

#endif /* INCLUDED_HCNSE_STRING_H */
