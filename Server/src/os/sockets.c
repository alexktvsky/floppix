#include "platform.h"
#include "error_proc.h"
#include "sockets.h"


int tcp_nopush(socket_t socket)
{
#if SYSTEM_LINUX
    int cork = 1;
    return setsockopt(socket, IPPROTO_TCP, TCP_CORK,
                        (const void *) &cork, sizeof(int));
/*
 *  int option = 1;
 *  return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
 *                     (const void *) &option, sizeof(int));
 */
#elif SYSTEM_FREEBSD
    int tcp_nopush = 1;
    return setsockopt(socket, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &tcp_nopush, sizeof(int));
#elif SYSTEM_SOLARIS
    /* do smth */
#else
    (void) socket;
    return XXX_OK;
#endif
}


int tcp_push(socket_t socket)
{
#if SYSTEM_LINUX
    int cork = 0;
    return setsockopt(socket, IPPROTO_TCP, TCP_CORK,
                        (const void *) &cork, sizeof(int));
#elif SYSTEM_FREEBSD
    int tcp_nopush = 0;
    return setsockopt(socket, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &tcp_nopush, sizeof(int));
#elif SYSTEM_SOLARIS
    /* do smth */
#else
    (void) socket;
    return XXX_OK;
#endif
}


int close_socket(socket_t socket)
{
#if SYSTEM_LINUX
    return close(socket);
#elif SYSTEM_FREEBSD
    return close(socket);
#elif SYSTEM_SOLARIS
    return close(socket);
#elif SYSTEM_WIN32 || SYSTEM_WIN64
    return closesocket(socket);
#endif
}


int init_winsock(void)
{
#if SYSTEM_WIN32 || SYSTEM_WIN64
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    return WSAStartup(wVersionRequested, &wsaData);
#else
    return XXX_OK;
#endif
}
