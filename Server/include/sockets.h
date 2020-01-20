#ifndef XXX_SOCKETS_H
#define XXX_SOCKETS_H

#include "platform.h"

#if (SYSTEM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#define SHUT_RECEIVE SHUT_RD
#define SHUT_SEND    SHUT_WR
#define SHUT_BOTH    SHUT_RDWR
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_FREEBSD)
/* Details for FreeBSD */
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_SOLARIS)
/* Details for Solaris */
#define INVALID_SOCKET -1
typedef int socket_t;


#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#include <ws2ipdef.h>
#include <windows.h>
#define SHUT_RECEIVE SD_RECEIVE
#define SHUT_SEND    SD_SEND
#define SHUT_BOTH    SD_BOTH
typedef SOCKET socket_t;

#else
#error "Undefined operating system!"
#endif


int tcp_nopush(socket_t socket);
int tcp_push(socket_t socket);
int close_socket(socket_t socket);
int init_winsock(void);

#endif /* XXX_SOCKETS_H */
