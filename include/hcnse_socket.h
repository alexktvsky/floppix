#ifndef INCLUDED_HCNSE_SOCKET_H
#define INCLUDED_HCNSE_SOCKET_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_LINUX)
#define HCNSE_SHUT_RD                  SHUT_RD
#define HCNSE_SHUT_WR                  SHUT_WR
#define HCNSE_SHUT_RDWR                SHUT_RDWR
#define HCNSE_INVALID_SOCKET           -1
#define HCNSE_FMT_SOCKET_T             "%d"

#elif (HCNSE_FREEBSD)
#define HCNSE_SHUT_RD                  SHUT_RD
#define HCNSE_SHUT_WR                  SHUT_WR
#define HCNSE_SHUT_RDWR                SHUT_RDWR
#define HCNSE_INVALID_SOCKET           -1
#define HCNSE_FMT_SOCKET_T             "%d"

#elif (HCNSE_SOLARIS)
/* Details for Solaris */
#define HCNSE_INVALID_SOCKET           -1

#elif (HCNSE_DARWIN)
#define HCNSE_SHUT_RD                  SHUT_RD
#define HCNSE_SHUT_WR                  SHUT_WR
#define HCNSE_SHUT_RDWR                SHUT_RDWR
#define HCNSE_INVALID_SOCKET           -1
#define HCNSE_FMT_SOCKET_T             "%d"

#elif (HCNSE_WIN32)
#define HCNSE_SHUT_RD                  SD_RECEIVE
#define HCNSE_SHUT_WR                  SD_SEND
#define HCNSE_SHUT_RDWR                SD_BOTH
#define HCNSE_INVALID_SOCKET           INVALID_SOCKET

#if (HCNSE_PTR_SIZE == 4)
#define HCNSE_FMT_SOCKET_T             "%d"
#else
#define HCNSE_FMT_SOCKET_T             "%I64d"
#endif

hcnse_err_t hcnse_winsock_init_v22(void);

#endif

hcnse_err_t hcnse_socket_init(hcnse_socket_t *sockfd, int domain, int type,
    int protocol);
hcnse_err_t hcnse_socket_bind(hcnse_socket_t sockfd,
    const struct sockaddr *addr, hcnse_socklen_t addrlen);
hcnse_err_t hcnse_socket_listen(hcnse_socket_t sockfd, int backlog);
hcnse_err_t hcnse_socket_accept(hcnse_socket_t *new_sockfd,
    hcnse_socket_t sockfd, struct sockaddr *addr, hcnse_socklen_t *addrlen);
hcnse_err_t hcnse_socket_connect(hcnse_socket_t sockfd,
    const struct sockaddr *addr, hcnse_socklen_t addrlen);
hcnse_err_t hcnse_socket_shutdown(hcnse_socket_t sockfd, int how);
void hcnse_socket_close(hcnse_socket_t sockf);

#endif /* INCLUDED_HCNSE_SOCKET_H */
