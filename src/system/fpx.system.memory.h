#ifndef FPX_SYSTEM_MEMORY_H
#define FPX_SYSTEM_MEMORY_H

#include "fpx.system.os.portable.h"
#include "fpx.system.type.h"

#define FPX_PTR_SIZE                 __SIZEOF_POINTER__
#define FPX_PTR_WIDTH                (8 * FPX_PTR_SIZE)
#define FPX_ALIGN_SIZE               sizeof(uintptr_t)
#define FPX_PAGE_SIZE                fpx_get_page_size()

#define fpx_align(p, b)  (((p) + ((b) - 1)) & ~((b) - 1))
#define fpx_align_default(p)         fpx_align(p, FPX_ALIGN_SIZE)

#define fpx_memset(buf, c, n)        memset(buf, c, n)
#define fpx_memzero(buf, n)          fpx_memset(buf, 0, n)
#define fpx_memcmp(s1, s2, n)        memcmp(s1, s2, n)
#define fpx_memcpy(dst, src, n)      memcpy(dst, src, n)
#define fpx_memmove(dst, src, n)     memmove(dst, src, n)


void *fpx_malloc(fpx_size_t size);
void *fpx_calloc(fpx_size_t size);
void fpx_free(void *mem);
fpx_size_t fpx_get_memory_counter(void);

void fpx_explicit_memzero(void *buf, fpx_size_t n);
fpx_size_t fpx_get_page_size(void);

#endif /* FPX_SYSTEM_MEMORY_H */
