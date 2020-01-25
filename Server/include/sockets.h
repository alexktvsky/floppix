#ifndef INCLUDED_SOCKETS_H
#define INCLUDED_SOCKETS_H

#include "syshead.h"

/* Some operating systems do not support a value more than 5 */
#define MAX_CONNECT_QUEUELEN  5

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

#define SHUT_RECEIVE SHUT_RD
#define SHUT_SEND    SHUT_WR
#define SHUT_BOTH    SHUT_RDWR
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_FREEBSD)
#include <sys/ioctl.h>
/* Details for FreeBSD */
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_SOLARIS)
#include <sys/ioctl.h>
/* Details for Solaris */
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#define SHUT_RECEIVE SD_RECEIVE
#define SHUT_SEND    SD_SEND
#define SHUT_BOTH    SD_BOTH
typedef SOCKET socket_t;

int init_winsock(void);

#endif

int tcp_nopush(socket_t s);
int tcp_push(socket_t s);
int socket_nonblocking(socket_t s);
int socket_blocking(socket_t s);
int close_socket(socket_t s);

#endif /* INCLUDED_SOCKETS_H */
