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

typedef unsigned int uint;

#endif /* FLOPPIX_SYSTEM_TYPE_H */
