#ifndef INCLUDED_OS_SOCKETS_H
#define INCLUDED_OS_SOCKETS_H

#include "os/syshead.h"

/* Some operating systems do not support a value more than 5 */
#define HCNSE_MAX_CONNECT_QUEUELEN     5

#if (HCNSE_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>

#define HCNSE_SHUT_RD                  SHUT_RD
#define HCNSE_SHUT_WR                  SHUT_WR
#define HCNSE_SHUT_RDWR                SHUT_RDWR
#define HCNSE_INVALID_SOCKET           -1
#define hcnse_close_socket(s)          close(s)
typedef int hcnse_socket_t;


#elif (HCNSE_FREEBSD)
#include <sys/ioctl.h>
/* Details for FreeBSD */
#define HCNSE_INVALID_SOCKET           -1
#define hcnse_close_socket(s)          close(s)
typedef int hcnse_socket_t;


#elif (HCNSE_SOLARIS)
#include <sys/ioctl.h>
/* Details for Solaris */
#define HCNSE_INVALID_SOCKET           -1
#define hcnse_close_socket(s)          close(s)
typedef int hcnse_socket_t;


#elif (HCNSE_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>

/* Need here to include winsock2.h before windows.h */
#include "server/errors.h"

#define HCNSE_SHUT_RD                  SD_RECEIVE
#define HCNSE_SHUT_WR                  SD_SEND
#define HCNSE_SHUT_RDWR                SD_BOTH
#define HCNSE_INVALID_SOCKET           INVALID_SOCKET
#define hcnse_close_socket(s)          closesocket(s)
typedef SOCKET hcnse_socket_t;

hcnse_err_t hcnse_winsock_init_v22(void);

#endif

#include "server/errors.h"

hcnse_err_t hcnse_tcp_nopush(hcnse_socket_t s);
hcnse_err_t hcnse_tcp_push(hcnse_socket_t s);
hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s);
hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s);

#endif /* INCLUDED_OS_SOCKETS_H */
