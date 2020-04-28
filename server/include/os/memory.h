#ifndef INCLUDED_OS_MEMORY_H
#define INCLUDED_OS_MEMORY_H

#include <stddef.h>

void hcnse_explicit_memzero(void *buf, size_t n);

void *hcnse_alloc(size_t size);
void *hcnse_calloc(size_t size);
void hcnse_free(void *mem);
size_t hcnse_total_mem_usage(void);


#endif /* INCLUDED_OS_MEMORY_H */
