#include "hcnse_portable.h"
#include "hcnse_core.h"


uint8_t
hcnse_get_number_cpu(void)
{
    int ncpus;
#if (HCNSE_POSIX)
#ifdef _SC_NPROCESSORS_ONLN
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#else
#error "Failed to determine number of CPUs"
#endif
#elif (HCNSE_WIN32)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    ncpus = info.dwNumberOfProcessors;
#endif
    if (ncpus <= 0) {
        ncpus = 1;
    }

    return ncpus;
}
