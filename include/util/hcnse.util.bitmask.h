#ifndef HCNSE_UTIL_BITMASK_H
#define HCNSE_UTIL_BITMASK_H

#include "hcnse.system.os.portable.h"

#define hcnse_bit_set(x, bit)     (x |= bit)
#define hcnse_bit_unset(x, bit)   (x &= ~(bit))
#define hcnse_bit_is_set(x, bit)  (x & bit)

typedef uint32_t                       hcnse_bitfield_t;

#endif /* HCNSE_UTIL_BITMASK_H */
