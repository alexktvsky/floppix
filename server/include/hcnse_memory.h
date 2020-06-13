#ifndef INCLUDED_HCNSE_MEMORY_H
#define INCLUDED_HCNSE_MEMORY_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Alignment macros is only to be used to align on a power of 2 boundary */
#define HCNSE_PTR_SIZE                 __SIZEOF_POINTER__
#define HCNSE_ALIGN_SIZE               sizeof(uintptr_t)

#define hcnse_align(p, b)  (((p) + ((b) - 1)) & ~((b) - 1))
#define hcnse_align_default(p)         hcnse_align(p, HCNSE_ALIGN_SIZE)

#define hcnse_memset(buf, c, n)        memset(buf, c, n)
#define hcnse_memzero(buf, n)          hcnse_memset(buf, 0, n)
#define hcnse_memcmp(s1, s2, n)        memcmp(s1, s2, n)
#define hcnse_memcpy(dst, src, n)      memcpy(dst, src, n)
#define hcnse_memmove(dst, src, n)     memmove(dst, src, n)


void *hcnse_malloc(size_t size);
void *hcnse_calloc(size_t size);
void hcnse_free(void *mem);
size_t hcnse_get_memory_counter(void);

void hcnse_explicit_memzero(void *buf, size_t n);
size_t hcnse_get_page_size(void);

#endif /* INCLUDED_HCNSE_MEMORY_H */
