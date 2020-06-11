#ifndef INCLUDED_HCNSE_STRING_H
#define INCLUDED_HCNSE_STRING_H

#define HCNSE_NULL                     0x0
#define HCNSE_TAB                      0x9
#define HCNSE_LF                       0xA
#define HCNSE_CR                       0xD
#define HCNSE_SPACE                    0x20

#define HCNSE_LF_STR                   "\n"
#define HCNSE_CR_STR                   "\r"
#define HCNSE_CRLF_STR                 "\r\n"

#if (HCNSE_POSIX)
#define HCNSE_EOL_STR                  HCNSE_LF_STR
#elif (HCNSE_WIN32)
#define HCNSE_EOL_STR                  HCNSE_CRLF_STR
#endif


#define hcnse_strlen(str)              strlen(str)
#define hcnse_memset(buf, c, n)        memset(buf, c, n)
#define hcnse_memcmp(s1, s2, n)        memcmp(s1, s2, n)
#define hcnse_memcpy(dst, src, n)      memcpy(dst, src, n)
#define hcnse_memmove(dst, src, n)     memmove(dst, src, n)
#define hcnse_strcmp(s1, s2)           strcmp(s1, s2)
#define hcnse_strncmp(s1, s2, n)       strncmp(s1, s2, n)
#define hcnse_atoi(str)                atoi(str)
#define hcnse_snprintf(str, size, ...) \
    snprintf(str, size, __VA_ARGS__)
#define hcnse_vsnprintf(str, size, ...) \
    vsnprintf(str, size, __VA_ARGS__)

#define hcnse_value(x)                 (#x)


#endif /* INCLUDED_HCNSE_STRING_H */
