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

static uint64_t hcnse_memory_counter_value;


void *
hcnse_malloc(size_t size)
{
    void *mem;

    mem = malloc(size);
    if (!mem) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "malloc(%zu) failed", size);
    }
    else {
        hcnse_memory_counter_value += size;
        hcnse_log_debug1(HCNSE_OK, "malloc %p:%zu total %zu",
            mem, size, hcnse_memory_counter_value);
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
    hcnse_log_debug1(HCNSE_OK, "free %p", mem);
}

size_t
hcnse_get_memory_counter(void)
{
    return hcnse_memory_counter_value;
}

void
hcnse_explicit_memzero(void *buf, size_t n)
{
    hcnse_memset(buf, 0, n);
    memory_barrier();
}


#if (HCNSE_POSIX)

size_t
hcnse_get_page_size(void)
{
    size_t page_size;

#if defined(_SC_PAGESIZE)
    page_size = sysconf(_SC_PAGESIZE);
#else
#error "Failed to determine page size"
#endif

    return page_size;
}

#elif (HCNSE_WIN32)

size_t
hcnse_get_page_size(void)
{
    SYSTEM_INFO si;
    size_t page_size;

    GetSystemInfo(&si);
    page_size = si.dwPageSize;

    return page_size;
}

#endif
