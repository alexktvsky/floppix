#ifndef INCLUDED_HCNSE_MEMORY_H
#define INCLUDED_HCNSE_MEMORY_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Alignment macros is only to be used to align on a power of 2 boundary */
#define HCNSE_ALIGN_SIZE sizeof(uintptr_t)
#define HCNSE_ALIGN(p, b) \
    (((p) + ((b) - 1)) & ~((b) - 1))
#define HCNSE_ALIGN_DEFAULT(p) HCNSE_ALIGN(p, HCNSE_ALIGN_SIZE)


void *hcnse_malloc(size_t size);
void *hcnse_calloc(size_t size);
void hcnse_free(void *mem);
size_t hcnse_total_mem_usage(void);

void hcnse_explicit_memzero(void *buf, size_t n);
size_t hcnse_get_page_size(void);

#endif /* INCLUDED_HCNSE_MEMORY_H */
