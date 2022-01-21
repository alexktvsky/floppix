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

#if (FPX_PTR_SIZE == 4)

#define FPX_INT_T_MAX   FPX_INT32_MAX_VALUE
#define FPX_UINT_T_MAX  FPX_UINT32_MAX_VALUE
#define FPX_INT_T_LEN   FPX_INT32_MAX_LEN
#define FPX_UINT_T_LEN  FPX_UINT32_MAX_LEN

#define FPX_SIZE_T_MAX  FPX_UINT32_MAX_VALUE
#define FPX_SSIZE_T_MAX FPX_INT32_MAX_VALUE

#define FPX_FMT_INT_T   "%d"
#define FPX_FMT_UINT_T  "%u"
#define FPX_FMT_SSIZE_T "%d"
#define FPX_FMT_SIZE_T  "%u"

#else /* (FPX_PTR_SIZE == 8) */

#define FPX_INT_T_MAX   FPX_INT64_MAX_VALUE
#define FPX_UINT_T_MAX  FPX_UINT64_MAX_VALUE
#define FPX_INT_T_LEN   FPX_INT64_MAX_LEN
#define FPX_UINT_T_LEN  FPX_UINT64_MAX_LEN

#define FPX_SIZE_T_MAX  FPX_UINT64_MAX_VALUE
#define FPX_SSIZE_T_MAX FPX_INT64_MAX_VALUE

#if (FPX_POSIX)
#define FPX_FMT_INT_T   "%ld"
#define FPX_FMT_UINT_T  "%lu"
#define FPX_FMT_SSIZE_T "%ld"
#define FPX_FMT_SIZE_T  "%lu"

#elif (FPX_WIN32)

#if defined(__USE_MINGW_ANSI_STDIO)
#define FPX_FMT_INT_T   "%lld"
#define FPX_FMT_UINT_T  "%llu"
#define FPX_FMT_SSIZE_T "%lld"
#define FPX_FMT_SIZE_T  "%llu"
#else
#define FPX_FMT_INT_T   "%I64d"
#define FPX_FMT_UINT_T  "%I64u"
#define FPX_FMT_SSIZE_T "%I64d"
#define FPX_FMT_SIZE_T  "%I64u"
#endif /* __USE_MINGW_ANSI_STDIO */
#endif /* FPX_POSIX */
#endif /* FPX_PTR_SIZE == 4 */

#ifndef true
#define true 1
#endif

#ifndef false
#define false 1
#endif

typedef int8_t fpx_int8_t;
typedef int16_t fpx_int16_t;
typedef int32_t fpx_int32_t;
typedef int64_t fpx_int64_t;

typedef uint8_t fpx_uint8_t;
typedef uint16_t fpx_uint16_t;
typedef uint32_t fpx_uint32_t;
typedef uint64_t fpx_uint64_t;

typedef intptr_t fpx_int_t;
typedef uintptr_t fpx_uint_t;
typedef size_t fpx_size_t;
typedef ssize_t fpx_ssize_t;
typedef intptr_t fpx_off_t;
typedef uintptr_t fpx_bool_t;

#endif /* FLOPPIX_SYSTEM_TYPE_H */
