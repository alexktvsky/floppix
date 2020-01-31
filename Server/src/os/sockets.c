#include "sockets.h"


#if (SYSTEM_LINUX)
int tcp_nopush(socket_t s)
{
    int option = 1;
    return setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int));
    // int  cork = 1;
    // return setsockopt(s, IPPROTO_TCP, TCP_CORK,
    //                      (const void *) &cork, sizeof(int));
}

int tcp_push(socket_t s)
{
    int option = 0;
    return setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int));
}

int socket_nonblocking(socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    return fcntl(s, F_SETFL, flags|O_NONBLOCK);
}

int socket_blocking(socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    return fcntl(s, F_SETFL, flags & (~O_NONBLOCK));
}


#elif (SYSTEM_FREEBSD)
int tcp_nopush(socket_t s)
{
    int tcp_nopush = 1;
    return setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &tcp_nopush, sizeof(int));
}

int tcp_push(socket_t s) {
    int tcp_nopush = 0;
    return setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &tcp_nopush, sizeof(int));
}

int socket_nonblocking(socket_t s)
{
    int nb = 1;
    return ioctl(s, FIONBIO, &nb);
}

int socket_blocking(socket_t s)
{
    int nb = 0;
    return ioctl(s, FIONBIO, &nb);
}


#elif (SYSTEM_SOLARIS)
int tcp_nopush(socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int tcp_push(socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

/* Setting FIONBIO or O_NDELAY on Solaris causes a read
 * with no data to return 0 on a tty or pipe, or -1 with
 * errno EAGAIN on a socket. However 0 is ambiguous since
 * it is also returned for EOF */
int socket_nonblocking(socket_t s)
{
    int nb = 1;
    return ioctl(s, FIONBIO, &nb);
}

int socket_blocking(socket_t s)
{
    int nb = 0;
    return ioctl(s, FIONBIO, &nb);
}


#elif (SYSTEM_WINDOWS)
int tcp_nopush(socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int tcp_push(socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int socket_nonblocking(socket_t s)
{
    DWORD nb = 1;
    return ioctlsocket(s, FIONBIO, &nb);
}

int socket_blocking(socket_t s)
{
    DWORD nb = 0;
    return ioctlsocket(s, FIONBIO, &nb);
}


int init_winsock(void)
{
    WSADATA data;
    WORD version_requested = MAKEWORD(2, 2);
    return WSAStartup(version_requested, &data);
}
#endif
