#ifndef FLOPPIX_SYSTEM_OS_PORTABLE_H
#define FLOPPIX_SYSTEM_OS_PORTABLE_H

#include "fpx_system_config.h"

#if (FPX_LINUX)
#include "floppix/system/os/linux.h"

#elif (FPX_FREEBSD)
#include "floppix/system/os/freebsd.h"

#elif (FPX_DARWIN)
#include "floppix/system/os/darwin.h"

#elif (FPX_SOLARIS)
#include "floppix/system/os/solaris.h"

#elif (FPX_WIN32)
#include "floppix/system/os/win32.h"

#endif

#endif /* FLOPPIX_SYSTEM_OS_PORTABLE_H */
