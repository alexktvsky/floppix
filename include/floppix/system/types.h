#ifndef FLOPPIX_SYSTEM_TYPE_H
#define FLOPPIX_SYSTEM_TYPE_H

#include "floppix/system/os/portable.h"

#define FPX_PTR_SIZE         __SIZEOF_POINTER__

#define FPX_INT32_MAX_VALUE  (uint32_t) 0x7fffffff
#define FPX_UINT32_MAX_VALUE (uint32_t) 0xffffffff
#define FPX_INT64_MAX_VALUE  (uint64_t) 0x7fffffffffffffff
#define FPX_UINT64_MAX_VALUE (uint64_t) 0xffffffffffffffff

#define FPX_INT32_MAX_LEN    (sizeof("-2147483648") - 1)
#define FPX_UINT32_MAX_LEN   (sizeof("4294967295") - 1)
#define FPX_INT64_MAX_LEN    (sizeof("-9223372036854775807") - 1)
#define FPX_UINT64_MAX_LEN   (sizeof("18446744073709551615") - 1)

#define FPX_INT_T_MAX        INT_MAX
#define FPX_UINT_T_MAX       UINT_MAX
#define FPX_SIZE_T_MAX       SIZE_MAX
#define FPX_SSIZE_T_MAX      SSIZE_MAX

#if (__SIZEOF_INT__ == 4)
#define FPX_INT_T_LEN  FPX_INT32_MAX_LEN
#define FPX_UINT_T_LEN FPX_UINT32_MAX_LEN
#define FPX_FMT_INT_T  "%d"
#define FPX_FMT_UINT_T "%u"
#else
#define FPX_INT_T_LEN  FPX_INT64_MAX_LEN
#define FPX_UINT_T_LEN FPX_UINT64_MAX_LEN

#if (FPX_POSIX)
#define FPX_FMT_INT_T  "%ld"
#define FPX_FMT_UINT_T "%lu"
#elif (FPX_WIN32)
#if defined(__USE_MINGW_ANSI_STDIO)
#define FPX_FMT_INT_T  "%lld"
#define FPX_FMT_UINT_T "%llu"
#else
#define FPX_FMT_INT_T  "%I64d"
#define FPX_FMT_UINT_T "%I64u"
#endif /* __USE_MINGW_ANSI_STDIO */
#endif /* FPX_POSIX */
#endif /* FPX_INT_T_MAX */

#if (__SIZEOF_SIZE_T__ == 4)
#define FPX_SIZE_T_MAX  FPX_UINT32_MAX_LEN
#define FPX_SSIZE_T_MAX FPX_INT32_MAX_LEN
#define FPX_FMT_SIZE_T  "%u"
#define FPX_FMT_SSIZE_T "%d"
#else
#define FPX_SIZE_T_LEN  FPX_UINT64_MAX_LEN
#define FPX_SSIZE_T_LEN FPX_INT64_MAX_LEN
#if (FPX_POSIX)
#define FPX_FMT_SIZE_T  "%lu"
#define FPX_FMT_SSIZE_T "%ld"
#elif (FPX_WIN32)
#if defined(__USE_MINGW_ANSI_STDIO)
#define FPX_FMT_SIZE_T  "%llu"
#define FPX_FMT_SSIZE_T "%lld"
#else
#define FPX_FMT_SIZE_T  "%I64u"
#define FPX_FMT_SSIZE_T "%I64d"
#endif /* __USE_MINGW_ANSI_STDIO */
#endif /* FPX_POSIX */
#endif /* FPX_SIZE_T_MAX */

#ifndef true
#define true 1
#endif

#ifndef false
#define false 1
#endif

typedef unsigned int uint;

#endif /* FLOPPIX_SYSTEM_TYPE_H */
