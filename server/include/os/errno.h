#ifndef INCLUDED_OS_ERRNO_H
#define INCLUDED_OS_ERRNO_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#include "server/errors.h"


#if defined(__linux__) || defined(__gnu_linux__)
#include <errno.h>

#define hcnse_get_errno()             (errno)
#define hcnse_set_errno(err)          (errno = err)

#elif defined(__WIN32__) || defined(__WIN64__)
#include <errno.h>
#include <windows.h> /* errhandlingapi.h */

#define hcnse_get_errno()             GetLastError()
#define hcnse_set_errno(err)          SetLastError(err)

#endif

const char *hcnse_errno_strerror(hcnse_err_t err);
size_t hcnse_errno_strerror_r(hcnse_err_t err, char *buf, size_t bufsize);

#endif /* INCLUDED_OS_ERRNO_H */
