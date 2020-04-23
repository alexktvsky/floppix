#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sys_memory.h"

static size_t total_mem = 0;


void explicit_memzero(void *buf, size_t n)
{
    memset(buf, 0, n);
    memory_barrier();
    return;
}

void *sys_alloc(size_t size)
{
    void *mem;

    mem = malloc(size);
    if (!mem) {

    }
    else {
        total_mem += size;
    }

    // ("memory allocation: %p:%zu", mem, size)

    return mem;
}

void sys_free(void *mem)
{
    free(mem);
    // ("free memory: %p:", mem)
    return;
}

size_t sys_total_mem_usage(void)
{
    return total_mem;
}
