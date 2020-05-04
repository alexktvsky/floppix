#ifndef INCLUDED_HCNSE_ERROR_H
#define INCLUDED_HCNSE_ERROR_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
#define hcnse_get_errno()              (errno)
#define hcnse_set_errno(err)           (errno = err)
#define hcnse_get_socket_errno()       (errno)
#define hcnse_set_socket_errno()       (errno = err)
#elif (HCNSE_WIN32)
#define hcnse_get_errno()              GetLastError()
#define hcnse_set_errno(err)           SetLastError(err)
#define hcnse_get_socket_errno()       WSAGetLastError()
#define hcnse_set_socket_errno()       WSASetLastError()
#endif

typedef int hcnse_errno_t;

const char *hcnse_errno_strerror(hcnse_errno_t err);
size_t hcnse_errno_strerror_r(hcnse_errno_t err, char *buf, size_t bufsize);

#endif /* INCLUDED_HCNSE_ERROR_H */
