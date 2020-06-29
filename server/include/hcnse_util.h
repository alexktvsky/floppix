#ifndef INCLUDED_HCNSE_UTIL_H
#define INCLUDED_HCNSE_UTIL_H


#define hcnse_flag_set(x, flag)     (x |= flag)
#define hcnse_flag_unset(x, flag)   (x &= ~(flag))
#define hcnse_flag_is_set(x, flag)  (x & flag)

#endif /* INCLUDED_HCNSE_UTIL_H */
