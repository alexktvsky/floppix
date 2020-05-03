#ifndef INCLUDED_HCNSE_PORTABLE_H
#define INCLUDED_HCNSE_PORTABLE_H

#if defined(__linux__) || defined(__gnu_linux__)
#define HCNSE_SYSTEM_NAME "Linux"
#define HCNSE_LINUX   1
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 0
#define HCNSE_UNIX    1
#define HCNSE_WINDOWS 0

#elif defined(__FreeBSD__)
#define HCNSE_SYSTEM_NAME "FreeBSD"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 1
#define HCNSE_SOLARIS 0
#define HCNSE_UNIX    1
#define HCNSE_WINDOWS 0

#elif defined(__sun) && defined(__SVR4)
#define HCNSE_SYSTEM_NAME "Solaris"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 1
#define HCNSE_UNIX    1
#define HCNSE_WINDOWS 0

#elif defined(__WIN32__) || defined(__WIN64__)
#define HCNSE_SYSTEM_NAME "Windows"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 0
#define HCNSE_UNIX    0
#define HCNSE_WINDOWS 1

#else
#error "Unsupported operating system"
#endif


#if (HCNSE_LINUX)
#include "hcnse_portable_linux.h"

#elif (HCNSE_FREEBSD)
#include "hcnse_portable_freebsd.h"

#elif (HCNSE_SOLARIS)
#include "hcnse_portable_solaris.h"

#elif (HCNSE_WINDOWS)
#include "hcnse_portable_windows.h"

#endif

#endif /* INCLUDED_HCNSE_PORTABLE_H */
