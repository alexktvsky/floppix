#ifndef INCLUDED_HCNSE_ERRNO_H
#define INCLUDED_HCNSE_ERRNO_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_POSIX)
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

#define HCNSE_OK                       0
#define HCNSE_FAILED                  -1
#define HCNSE_BUSY                    -2
#define HCNSE_ABORT                   -3

/* Where the HCNSE specific error values start */
#define HCNSE_ERROR_DOMAIN_BASE        20000
#define HCNSE_ERRSPACE_SIZE            100
#define HCNSE_ERROR_DOMAIN(domain) \
    (HCNSE_ERROR_DOMAIN_BASE + (domain * HCNSE_ERRSPACE_SIZE))


#define HCNSE_ERR_CONF                 HCNSE_ERROR_DOMAIN(1)
#define HCNSE_ERR_CONF_REGEX           (HCNSE_ERR_CONF+1)

#define HCNSE_ERR_NET                  HCNSE_ERROR_DOMAIN(2)
#define HCNSE_ERR_NET_GAI              (HCNSE_ERR_NET+1)
#define HCNSE_ERR_NET_GSN              (HCNSE_ERR_NET+2)
#define HCNSE_ERR_NET_GPN              (HCNSE_ERR_NET+3)

#define HCNSE_ERR_SSL                  HCNSE_ERROR_DOMAIN(3)
#define HCNSE_ERR_SSL_INIT             (HCNSE_ERR_SSL+1)


const char *hcnse_strerror(hcnse_err_t errcode);
size_t hcnse_strerror_r(hcnse_err_t errcode, char *buf, size_t bufsize);
const char *hcnse_errno_strerror(hcnse_errno_t err);
size_t hcnse_errno_strerror_r(hcnse_errno_t err, char *buf, size_t bufsize);

#endif /* INCLUDED_HCNSE_ERRNO_H */
