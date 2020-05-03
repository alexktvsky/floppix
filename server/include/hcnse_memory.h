#ifndef INCLUDED_HCNSE_MEMORY_H
#define INCLUDED_HCNSE_MEMORY_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

void *hcnse_malloc(size_t size);
void *hcnse_calloc(size_t size);
void hcnse_free(void *mem);
size_t hcnse_total_mem_usage(void);

void hcnse_explicit_memzero(void *buf, size_t n);
size_t hcnse_get_page_size(void);

#endif /* INCLUDED_HCNSE_MEMORY_H */
