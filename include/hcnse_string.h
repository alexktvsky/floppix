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

#define hcnse_strcmp(s1, s2)           strcmp(s1, s2)
#define hcnse_strncmp(s1, s2, n)       strncmp(s1, s2, n)
#define hcnse_snprintf(str, size, ...) \
    snprintf(str, size, __VA_ARGS__)
#define hcnse_vsnprintf(str, size, ...) \
    vsnprintf(str, size, __VA_ARGS__)

#define hcnse_isdigit(c) \
    ((c >= '0') && (c <= '9'))

#define hcnse_isalpha(c) \
    (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))

#define hcnse_isascii(c) \
    ((((int8_t) c) >= 0x0) && (((uint8_t) c) <= 0x7f))

#define hcnse_tolower(c) \
    ((char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c))

#define hcnse_toupper(c) \
    ((char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c))


#define hcnse_value_helper(x)          #x
#define hcnse_value(x)                 hcnse_value_helper(x)
#define hcnse_stringify(x)             hcnse_value_helper(x)

#define hcnse_pstrcat(...)             hcnse_pstrcat1(__VA_ARGS__, NULL)

hcnse_int_t hcnse_atoi(const char *str, size_t n);
ssize_t hcnse_atosz(const char *str, size_t n);
size_t hcnse_strlen(const char *str);
size_t hcnse_strnlen(const char *str, size_t n);
hcnse_int_t hcnse_strcasecmp(char *str1, char *str2);
hcnse_int_t hcnse_strncasecmp(char *str1, char *str2, size_t n);
char *hcnse_strchr(const char *str, int c);
char *hcnse_strrchr(const char *str, int c);
char *hcnse_pstrdup(hcnse_pool_t *pool, const char *str);
char *hcnse_pstrndup(hcnse_pool_t *pool, const char *str, size_t n);
char *hcnse_pstrcat1(hcnse_pool_t *pool, ...);

#endif /* INCLUDED_HCNSE_STRING_H */
