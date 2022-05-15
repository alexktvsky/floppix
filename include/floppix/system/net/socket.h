#ifndef FLOPPIX_SYSTEM_NET_SOCKET_H
#define FLOPPIX_SYSTEM_NET_SOCKET_H

#include "floppix/system/os/portable.h"
#include "floppix/system/errno.h"

#if (FPX_LINUX)
#define FPX_SHUT_RD        SHUT_RD
#define FPX_SHUT_WR        SHUT_WR
#define FPX_SHUT_RDWR      SHUT_RDWR
#define FPX_INVALID_SOCKET -1
#define FPX_FMT_SOCKET_T   "%i"

#elif (FPX_FREEBSD)
#define FPX_SHUT_RD        SHUT_RD
#define FPX_SHUT_WR        SHUT_WR
#define FPX_SHUT_RDWR      SHUT_RDWR
#define FPX_INVALID_SOCKET -1
#define FPX_FMT_SOCKET_T   "%i"

#elif (FPX_SOLARIS)
/* Details for Solaris */
#define FPX_INVALID_SOCKET -1

#elif (FPX_DARWIN)
#define FPX_SHUT_RD        SHUT_RD
#define FPX_SHUT_WR        SHUT_WR
#define FPX_SHUT_RDWR      SHUT_RDWR
#define FPX_INVALID_SOCKET -1
#define FPX_FMT_SOCKET_T   "%i"

#elif (FPX_WIN32)
#define FPX_SHUT_RD        SD_RECEIVE
#define FPX_SHUT_WR        SD_SEND
#define FPX_SHUT_RDWR      SD_BOTH
#define FPX_INVALID_SOCKET INVALID_SOCKET

#if (FPX_PTR_SIZE == 4)
#define FPX_FMT_SOCKET_T "%i"
#else
#define FPX_FMT_SOCKET_T "%I64d"
#endif

fpx_err_t fpx_winsock_init_v22(void);

#endif

fpx_err_t fpx_socket_init(fpx_socket_t *sockfd, int domain, int type,
    int protocol);
fpx_err_t fpx_socket_bind(fpx_socket_t sockfd, const struct sockaddr *addr,
    fpx_socklen_t addrlen);
fpx_err_t fpx_socket_listen(fpx_socket_t sockfd, int backlog);
fpx_err_t fpx_socket_accept(fpx_socket_t *new_sockfd, fpx_socket_t sockfd,
    struct sockaddr *addr, fpx_socklen_t *addrlen);
fpx_err_t fpx_socket_connect(fpx_socket_t sockfd, const struct sockaddr *addr,
    fpx_socklen_t addrlen);
fpx_err_t fpx_socket_shutdown(fpx_socket_t sockfd, int how);
void fpx_socket_close(fpx_socket_t sockf);

#endif /* FLOPPIX_SYSTEM_NET_SOCKET_H */
