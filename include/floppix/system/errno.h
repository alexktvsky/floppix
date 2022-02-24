#ifndef FLOPPIX_SYSTEM_ERRNO_H
#define FLOPPIX_SYSTEM_ERRNO_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"

#define FPX_ERRNO_STR_SIZE 1024

#if (FPX_POSIX)
#define fpx_get_errno()        (errno)
#define fpx_set_errno(err)     (errno = err)
#define fpx_get_socket_errno() (errno)
#define fpx_set_socket_errno() (errno = err)

#elif (FPX_WIN32)
#define fpx_get_errno()           GetLastError()
#define fpx_set_errno(err)        SetLastError(err)
#define fpx_get_socket_errno()    WSAGetLastError()
#define fpx_set_socket_errno(err) WSASetLastError(err)
#endif

#define FPX_OK                0
#define FPX_FAILED            -1
#define FPX_BUSY              -2
#define FPX_DONE              -3
#define FPX_ABORT             -4
#define FPX_DECLINED          -5
#define FPX_NOT_FOUND         -6

/* Where the FPX specific error values start */
#define FPX_ERROR_DOMAIN_BASE 20000
#define FPX_ERRSPACE_SIZE     100
#define FPX_ERROR_DOMAIN(domain)                                               \
    (FPX_ERROR_DOMAIN_BASE + (domain * FPX_ERRSPACE_SIZE))

#define FPX_ERR_CONFIG            FPX_ERROR_DOMAIN(1)
#define FPX_ERR_CONFIG_SYNTAX     (FPX_ERR_CONFIG + 1)

#define FPX_ERR_FILESYS           FPX_ERROR_DOMAIN(2)
#define FPX_ERR_FILESYS_ABS_PATH  (FPX_ERR_FILESYS + 1)
#define FPX_ERR_FILESYS_LONG_PATH (FPX_ERR_FILESYS + 2)

#define FPX_ERR_SSL               FPX_ERROR_DOMAIN(3)
#define FPX_ERR_SSL_INIT          (FPX_ERR_SSL + 1)

typedef int fpx_err_t;

const char *fpx_strerror(fpx_err_t err, char *buf, size_t bufsize);

#endif /* FLOPPIX_SYSTEM_ERRNO_H */
