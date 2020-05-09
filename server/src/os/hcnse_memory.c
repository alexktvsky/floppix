#include "hcnse_portable.h"
#include "hcnse_core.h"

#if defined(__GNUC__)
#define memory_barrier() __sync_synchronize()
#elif defined(__sparc) || defined(__sparc__)
#define memory_barrier() __asm (".volatile"); __asm (".nonvolatile")
#else
#define memory_barrier()
/* TODO: etc */
#endif

static size_t total_mem = 0;


void *
hcnse_malloc(size_t size)
{
    void *mem;

    mem = malloc(size);
    if (mem) {
        total_mem += size;
    }

    return mem;
}

void *
hcnse_calloc(size_t size)
{
    void *mem;

    mem = hcnse_malloc(size);
    if (!mem) {
        return mem;
    }
    hcnse_memset(mem, 0, size);
    return mem;
}


void
hcnse_free(void *mem)
{
    free(mem);
}

size_t
hcnse_get_total_mem_usage(void)
{
    return total_mem;
}

void
hcnse_explicit_memzero(void *buf, size_t n)
{
    hcnse_memset(buf, 0, n);
    memory_barrier();
}

size_t
hcnse_get_page_size(void)
{
    size_t page_size;
#if defined(_SC_PAGESIZE)
    page_size = sysconf(_SC_PAGESIZE);
#elif (HCNSE_WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    page_size = si.dwPageSize;
#else
#error "Failed to determine page size"
#endif
    return page_size;
}
