#ifndef INCLUDED_SYS_MEMORY_H
#define INCLUDED_SYS_MEMORY_H

#include <stddef.h>

void explicit_memzero(void *buf, size_t n);

void *sys_alloc(size_t size);
void sys_free(void *mem);
size_t sys_total_mem_usage(void);


#endif /* INCLUDED_SYS_MEMORY_H */
