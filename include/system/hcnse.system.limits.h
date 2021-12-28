#ifndef HCNSE_SYSTEM_LIMITS_H
#define HCNSE_SYSTEM_LIMITS_H


#define HCNSE_INT32_MAX_VALUE          (uint32_t) 0x7fffffff
#define HCNSE_UINT32_MAX_VALUE         (uint32_t) 0xffffffff

#define HCNSE_INT64_MAX_VALUE          (uint64_t) 0x7fffffffffffffff
#define HCNSE_UINT64_MAX_VALUE         (uint64_t) 0xffffffffffffffff


#define HCNSE_INT32_MAX_LEN            (sizeof("-2147483648") - 1)
#define HCNSE_UINT32_MAX_LEN           (sizeof("4294967295") - 1)

#define HCNSE_INT64_MAX_LEN            (sizeof("-9223372036854775807") - 1)
#define HCNSE_UINT64_MAX_LEN           (sizeof("18446744073709551615") - 1)

#if (HCNSE_PTR_SIZE == 4)

#define HCNSE_INT_T_MAX                HCNSE_INT32_MAX_VALUE
#define HCNSE_UINT_T_MAX               HCNSE_UINT32_MAX_VALUE
#define HCNSE_INT_T_LEN                HCNSE_INT32_MAX_LEN
#define HCNSE_UINT_T_LEN               HCNSE_UINT32_MAX_LEN

#define HCNSE_SIZE_T_MAX               HCNSE_UINT32_MAX_VALUE
#define HCNSE_SSIZE_T_MAX              HCNSE_INT32_MAX_VALUE

#define HCNSE_FMT_INT_T                "%d"
#define HCNSE_FMT_UINT_T               "%u"
#define HCNSE_FMT_SSIZE_T              "%d"
#define HCNSE_FMT_SIZE_T               "%u"

#else
#define HCNSE_INT_T_MAX                HCNSE_INT64_MAX_VALUE
#define HCNSE_UINT_T_MAX               HCNSE_UINT64_MAX_VALUE
#define HCNSE_INT_T_LEN                HCNSE_INT64_MAX_LEN
#define HCNSE_UINT_T_LEN               HCNSE_UINT64_MAX_LEN

#define HCNSE_SIZE_T_MAX               HCNSE_UINT64_MAX_VALUE
#define HCNSE_SSIZE_T_MAX              HCNSE_INT64_MAX_VALUE

#if (HCNSE_POSIX)
#define HCNSE_FMT_INT_T                "%ld"
#define HCNSE_FMT_UINT_T               "%lu"
#define HCNSE_FMT_SSIZE_T              "%ld"
#define HCNSE_FMT_SIZE_T               "%lu"

#elif (HCNSE_WIN32)

#if defined(__USE_MINGW_ANSI_STDIO)
#define HCNSE_FMT_INT_T                "%lld"
#define HCNSE_FMT_UINT_T               "%llu"
#define HCNSE_FMT_SSIZE_T              "%lld"
#define HCNSE_FMT_SIZE_T               "%llu"
#else
#define HCNSE_FMT_INT_T                "%I64d"
#define HCNSE_FMT_UINT_T               "%I64u"
#define HCNSE_FMT_SSIZE_T              "%I64d"
#define HCNSE_FMT_SIZE_T               "%I64u"
#endif

#endif

#endif

#endif /* HCNSE_SYSTEM_LIMITS_H */
