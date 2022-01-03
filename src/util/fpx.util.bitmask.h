#ifndef FPX_UTIL_BITMASK_H
#define FPX_UTIL_BITMASK_H

#include "fpx.system.os.portable.h"

#define fpx_bit_set(x, bit)    (x |= bit)
#define fpx_bit_unset(x, bit)  (x &= ~(bit))
#define fpx_bit_is_set(x, bit) (x & bit)

typedef uint32_t fpx_bitmask_t;

#endif /* FPX_UTIL_BITMASK_H */
