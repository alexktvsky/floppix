#include "floppix/system/cpuinfo.h"

#if (FPX_POSIX)

fpx_uint_t
fpx_get_number_cpu(void)
{
    fpx_int_t ncpus;

#if (FPX_HAVE_SC_NPROC)
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#else
    ncpus = 1;
#endif

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (fpx_uint_t) ncpus;
}

#elif (FPX_WIN32)

fpx_uint_t
fpx_get_number_cpu(void)
{
    SYSTEM_INFO info;
    fpx_int_t ncpus;

    GetSystemInfo(&info);
    ncpus = info.dwNumberOfProcessors;

    if (ncpus <= 0) {
        ncpus = 1;
    }

    return (fpx_uint_t) ncpus;
}

#endif
