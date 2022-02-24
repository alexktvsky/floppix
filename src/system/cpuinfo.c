#include "floppix/system/cpuinfo.h"

#if (FPX_POSIX)

uint
fpx_get_number_cpu(void)
{
    int ncpus;

#if (FPX_HAVE_SC_NPROC)
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#else
    ncpus = 1;
#endif

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (uint) ncpus;
}

#elif (FPX_WIN32)

uint
fpx_get_number_cpu(void)
{
    SYSTEM_INFO info;
    int ncpus;

    GetSystemInfo(&info);
    ncpus = info.dwNumberOfProcessors;

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (uint) ncpus;
}

#endif
