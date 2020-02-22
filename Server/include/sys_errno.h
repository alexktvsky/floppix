#ifndef INCLUDED_ERRNO_H
#define INCLUDED_ERRNO_H

#if defined(__linux__) || defined(__gnu_linux__)
#include <errno.h>

#define sys_get_errno               errno
#define sys_set_errno(err)          (errno = err)

#elif defined(__WIN32__) || defined(__WIN64__)
#include <errno.h>
#include <windows.h> /* errhandlingapi.h */

#define sys_get_errno               GetLastError()
#define sys_set_errno(err)          SetLastError(err)

#endif

typedef int sys_err_t;

const char *sys_strerror(sys_err_t err);
size_t sys_cpystrerror(sys_err_t err, char *buf, size_t bufsize);
int sys_strerror_init(void);

#endif /* INCLUDED_ERRNO_H */
