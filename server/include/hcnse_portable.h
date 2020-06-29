#ifndef INCLUDED_HCNSE_PORTABLE_H
#define INCLUDED_HCNSE_PORTABLE_H

#if defined(__linux__) || defined(__gnu_linux__)
#define HCNSE_SYSTEM_NAME  "GNU/Linux"
#define HCNSE_LINUX    1
#define HCNSE_FREEBSD  0
#define HCNSE_DARWIN   0
#define HCNSE_SOLARIS  0
#define HCNSE_POSIX    1
#define HCNSE_WIN32    0

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define HCNSE_SYSTEM_NAME  "FreeBSD"
#define HCNSE_LINUX    0
#define HCNSE_FREEBSD  1
#define HCNSE_DARWIN   0
#define HCNSE_SOLARIS  0
#define HCNSE_POSIX    1
#define HCNSE_WIN32    0

#elif defined(__APPLE__) && defined(__MACH__)
#define HCNSE_SYSTEM_NAME  "Darwin"
#define HCNSE_LINUX    0
#define HCNSE_FREEBSD  0
#define HCNSE_DARWIN   1
#define HCNSE_SOLARIS  0
#define HCNSE_POSIX    1
#define HCNSE_WIN32    0

#elif defined(__sun) && defined(__SVR4)
#define HCNSE_SYSTEM_NAME  "Solaris"
#define HCNSE_LINUX    0
#define HCNSE_FREEBSD  0
#define HCNSE_DARWIN   0
#define HCNSE_SOLARIS  1
#define HCNSE_POSIX    1
#define HCNSE_WIN32    0

#elif defined(_WIN32) || defined(__WIN32__)
#define HCNSE_SYSTEM_NAME  "Win32"
#define HCNSE_LINUX    0
#define HCNSE_FREEBSD  0
#define HCNSE_DARWIN   0
#define HCNSE_SOLARIS  0
#define HCNSE_POSIX    0
#define HCNSE_WIN32    1

#else
#error "Unsupported operating system"
#endif


#if (HCNSE_LINUX)
#include "hcnse_portable_linux.h"

#elif (HCNSE_FREEBSD)
#include "hcnse_portable_freebsd.h"

#elif (HCNSE_DARWIN)
#include "hcnse_portable_darwin.h"

#elif (HCNSE_SOLARIS)
#include "hcnse_portable_solaris.h"

#elif (HCNSE_WIN32)
#include "hcnse_portable_win32.h"

#endif

typedef int                            hcnse_err_t;
typedef intptr_t                       hcnse_int_t;
typedef uintptr_t                      hcnse_uint_t;
typedef size_t                         hcnse_size_t;
typedef ssize_t                        hcnse_ssize_t;
typedef uint32_t                       hcnse_flag_t;
typedef uintptr_t                      hcnse_msec_t;
typedef struct hcnse_file_s            hcnse_file_t;
typedef struct hcnse_thread_s          hcnse_thread_t;
typedef struct hcnse_mutex_s           hcnse_mutex_t;
typedef struct hcnse_semaphore_s       hcnse_semaphore_t;

#endif /* INCLUDED_HCNSE_PORTABLE_H */
