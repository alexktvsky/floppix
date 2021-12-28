#include "hcnse.system.cpuinfo.h"

#if (HCNSE_POSIX)

hcnse_uint_t
hcnse_get_number_cpu(void)
{
    hcnse_int_t ncpus;

#if (HCNSE_HAVE_SC_NPROC)
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#else
    ncpus = 1;
#endif

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (hcnse_uint_t) ncpus;
}

#elif (HCNSE_WIN32)

hcnse_uint_t
hcnse_get_number_cpu(void)
{
    SYSTEM_INFO info;
    hcnse_int_t ncpus;

    GetSystemInfo(&info);
    ncpus = info.dwNumberOfProcessors;

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (hcnse_uint_t) ncpus;
}

#endif
