#include "floppix/system/memory.h"
#include "floppix/core/log.h"

#if defined(__GNUC__)
#define memory_barrier() __sync_synchronize()
#elif defined(__sparc) || defined(__sparc__)
#define memory_barrier()                                                       \
    __asm(".volatile");                                                        \
    __asm(".nonvolatile")
#else
#define memory_barrier()
/* TODO: etc */
#endif

static uint64_t fpx_memory_counter_value;

void *
fpx_malloc1(const char *filename, int line, size_t size)
{
    void *mem;

    mem = malloc(size);
    if (!mem) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(), "malloc(%zu) failed",
            size);
    }
    else {
        fpx_memory_counter_value += size;
        fpx_log_debug1(FPX_OK, "malloc %p %zu in %s:%u total %zu", mem, size,
            filename, line, fpx_memory_counter_value);
    }

    return mem;
}

void *
fpx_calloc1(const char *filename, int line, size_t size)
{
    void *mem;

    mem = fpx_malloc1(filename, line, size);
    if (!mem) {
        return mem;
    }
    fpx_memzero(mem, size);
    return mem;
}

void
fpx_free(void *mem)
{
    free(mem);
    fpx_log_debug1(FPX_OK, "free %p", mem);
}

size_t
fpx_get_memory_counter(void)
{
    return fpx_memory_counter_value;
}

void
fpx_explicit_memzero(void *buf, size_t n)
{
    fpx_memzero(buf, n);
    memory_barrier();
}

#if (FPX_POSIX)

size_t
fpx_get_page_size(void)
{
    size_t page_size;

#if (FPX_HAVE_SC_PAGESIZE)
    page_size = sysconf(_SC_PAGESIZE);
#else
    page_size = 4096;
#endif

    return page_size;
}

#elif (FPX_WIN32)

size_t
fpx_get_page_size(void)
{
    SYSTEM_INFO si;
    size_t page_size;

    GetSystemInfo(&si);
    page_size = si.dwPageSize;

    return page_size;
}

#endif
