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

static uint64_t hcnse_total_mem;


void *
hcnse_malloc(size_t size)
{
    void *mem;

    mem = malloc(size);
    if (!mem) {
        hcnse_log_error1(HCNSE_LOG_EMERG, hcnse_get_errno(),
            "malloc(%zu) failed", size);
    }
    else {
        hcnse_total_mem += size;
        hcnse_log_error1(HCNSE_LOG_DEBUG, HCNSE_OK,
            "malloc %p:%zu total %zu", mem, size, hcnse_total_mem);
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
    hcnse_log_error1(HCNSE_LOG_DEBUG, HCNSE_OK, "free %p", mem);
}

size_t
hcnse_get_hcnse_total_mem_usage(void)
{
    return hcnse_total_mem;
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
