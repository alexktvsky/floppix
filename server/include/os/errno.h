#ifndef INCLUDED_OS_ERRNO_H
#define INCLUDED_OS_ERRNO_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#include "os/syshead.h"
#include "server/errors.h"


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
#include <errno.h>

#define hcnse_get_errno()             (errno)
#define hcnse_set_errno(err)          (errno = err)

#elif (HCNSE_WINDOWS)
#include <errno.h>
#include <windows.h> /* errhandlingapi.h */

#define hcnse_get_errno()             GetLastError()
#define hcnse_set_errno(err)          SetLastError(err)

#endif

const char *hcnse_errno_strerror(hcnse_err_t err);
size_t hcnse_errno_strerror_r(hcnse_err_t err, char *buf, size_t bufsize);

#endif /* INCLUDED_OS_ERRNO_H */
