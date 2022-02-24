#ifndef FLOPPIX_SYSTEM_MEMORY_H
#define FLOPPIX_SYSTEM_MEMORY_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"

#define FPX_PTR_SIZE         __SIZEOF_POINTER__
#define FPX_PTR_WIDTH        (8 * FPX_PTR_SIZE)
#define FPX_ALIGN_SIZE       sizeof(uintptr_t)
#define FPX_PAGE_SIZE        fpx_get_page_size()

#define fpx_align(p, b)      (((p) + ((b) -1)) & ~((b) -1))
#define fpx_align_default(p) fpx_align(p, FPX_ALIGN_SIZE)

/*
 * #define fpx_offsetof(st, m) \
 *((size_t) ((uint8_t *) &((st *) 0)->m - (uint8_t *) 0))
 */
#define fpx_offsetof(st, m)  offsetof(st, m)

#define fpx_container_of(ptr, type, member)                                    \
    (type *) ((uint8_t *) ptr - fpx_offsetof(type, member))

#define fpx_memset(ptr, c, n)    memset(ptr, c, n)
#define fpx_memzero(ptr, n)      fpx_memset(ptr, 0, n)
#define fpx_memcmp(s1, s2, n)    memcmp(s1, s2, n)
#define fpx_memcpy(dst, src, n)  memcpy(dst, src, n)
#define fpx_memmove(dst, src, n) memmove(dst, src, n)
#define fpx_malloc(size)         fpx_malloc1(__FILE__, __LINE__, size)
#define fpx_calloc(size)         fpx_calloc1(__FILE__, __LINE__, size)

void *fpx_malloc1(const char *filename, int line, size_t size);
void *fpx_calloc1(const char *filename, int line, size_t size);
void fpx_free(void *mem);
size_t fpx_get_memory_counter(void);

void fpx_explicit_memzero(void *buf, size_t n);
size_t fpx_get_page_size(void);

#endif /* FLOPPIX_SYSTEM_MEMORY_H */
