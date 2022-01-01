#ifndef FPX_SYSTEM_OS_PORTABLE_H
#define FPX_SYSTEM_OS_PORTABLE_H

#include "fpx.system.os.config.h"

#if (FPX_LINUX)
#include "fpx.system.os.linux.h"

#elif (FPX_FREEBSD)
#include "fpx.system.os.freebsd.h"

#elif (FPX_DARWIN)
#include "fpx.system.os.darwin.h"

#elif (FPX_SOLARIS)
#include "fpx.system.os.solaris.h"

#elif (FPX_WIN32)
#include "fpx.system.os.win32.h"

#endif

#endif /* FPX_SYSTEM_OS_PORTABLE_H */
