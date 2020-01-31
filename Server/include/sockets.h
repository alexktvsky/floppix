#ifndef INCLUDED_SOCKETS_H
#define INCLUDED_SOCKETS_H

#include "syshead.h"

/* Some operating systems do not support a value more than 5 */
#define MAX_CONNECT_QUEUELEN         5

#if (SYSTEM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>

#define SYS_SHUT_RECEIVE             SHUT_RD
#define SYS_SHUT_SEND                SHUT_WR
#define SYS_SHUT_BOTH                SHUT_RDWR
#define SYS_INVALID_SOCKET          -1
#define close_socket(s)              close(s)
typedef int socket_t;


#elif (SYSTEM_FREEBSD)
#include <sys/ioctl.h>
/* Details for FreeBSD */
#define SYS_INVALID_SOCKET          -1
#define close_socket(s)              close(s)
typedef int socket_t;


#elif (SYSTEM_SOLARIS)
#include <sys/ioctl.h>
/* Details for Solaris */
#define SYS_INVALID_SOCKET          -1
#define close_socket(s)              close(s)
typedef int socket_t;


#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#define SYS_SHUT_RECEIVE             SD_RECEIVE
#define SYS_SHUT_SEND                SD_SEND
#define SYS_SHUT_BOTH                SD_BOTH
#define close_socket(s)              closesocket(s)
typedef SOCKET socket_t;

int init_winsock(void);

#endif

int tcp_nopush(socket_t s);
int tcp_push(socket_t s);
int socket_nonblocking(socket_t s);
int socket_blocking(socket_t s);

#endif /* INCLUDED_SOCKETS_H */
