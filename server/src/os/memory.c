#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "os/memory.h"

#if defined(__GNUC__)
#define memory_barrier() __sync_synchronize()
#elif defined(__sparc) || defined(__sparc__)
#define memory_barrier() __asm (".volatile"); __asm (".nonvolatile")
#else
#define memory_barrier()
/* TODO: etc */
#endif


static size_t total_mem = 0;

void hcnse_explicit_memzero(void *buf, size_t n)
{
    memset(buf, 0, n);
    memory_barrier();
}

void *hcnse_malloc(size_t size)
{
    void *mem;

    mem = malloc(size);
    if (mem) {
        total_mem += size;
    }

    return mem;
}

void *hcnse_calloc(size_t size)
{
    void *mem;

    mem = hcnse_malloc(size);
    if (!mem) {
        return mem;
    }
    memset(mem, 0, size);
    return mem;
}


void hcnse_free(void *mem)
{
    free(mem);
    // ("free memory: %p:", mem)
}

size_t hcnse_total_mem_usage(void)
{
    return total_mem;
}
