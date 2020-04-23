#ifndef INCLUDED_SYS_SOCKETS_H
#define INCLUDED_SYS_SOCKETS_H

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

#define SYS_SHUT_RD                  SHUT_RD
#define SYS_SHUT_WR                  SHUT_WR
#define SYS_SHUT_RDWR                SHUT_RDWR
#define SYS_INVALID_SOCKET           -1
#define close_socket(s)              close(s)
typedef int sys_socket_t;


#elif (SYSTEM_FREEBSD)
#include <sys/ioctl.h>
/* Details for FreeBSD */
#define SYS_INVALID_SOCKET           -1
#define close_socket(s)              close(s)
typedef int sys_socket_t;


#elif (SYSTEM_SOLARIS)
#include <sys/ioctl.h>
/* Details for Solaris */
#define SYS_INVALID_SOCKET           -1
#define close_socket(s)              close(s)
typedef int sys_socket_t;


#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#define SYS_SHUT_RD                  SD_RECEIVE
#define SYS_SHUT_WR                  SD_SEND
#define SYS_SHUT_RDWR                SD_BOTH
#define SYS_INVALID_SOCKET           INVALID_SOCKET
#define close_socket(s)              closesocket(s)
typedef SOCKET sys_socket_t;

int winsock_init_v22(void);

#endif

int tcp_nopush(sys_socket_t s);
int tcp_push(sys_socket_t s);
int socket_nonblocking(sys_socket_t s);
int socket_blocking(sys_socket_t s);

#endif /* INCLUDED_SYS_SOCKETS_H */
