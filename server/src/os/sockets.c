#include "os/sockets.h"


#if (HCNSE_LINUX)
int hcnse_tcp_nopush(hcnse_socket_t s)
{
    int option = 1;
    return setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int));
    // int  cork = 1;
    // return setsockopt(s, IPPROTO_TCP, TCP_CORK,
    //                      (const void *) &cork, sizeof(int));
}

int hcnse_tcp_push(hcnse_socket_t s)
{
    int option = 0;
    return setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int));
}

int hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    return fcntl(s, F_SETFL, flags|O_NONBLOCK);
}

int hcnse_socket_blocking(hcnse_socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    return fcntl(s, F_SETFL, flags & (~O_NONBLOCK));
}


#elif (HCNSE_FREEBSD)
int hcnse_tcp_nopush(hcnse_socket_t s)
{
    int hcnse_tcp_nopush = 1;
    return setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &hcnse_tcp_nopush, sizeof(int));
}

int hcnse_tcp_push(hcnse_socket_t s) {
    int hcnse_tcp_nopush = 0;
    return setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
                        (const void *) &hcnse_tcp_nopush, sizeof(int));
}

int hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int nb = 1;
    return ioctl(s, FIONBIO, &nb);
}

int hcnse_socket_blocking(hcnse_socket_t s)
{
    int nb = 0;
    return ioctl(s, FIONBIO, &nb);
}


#elif (HCNSE_SOLARIS)
int hcnse_tcp_nopush(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int hcnse_tcp_push(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

/* Setting FIONBIO or O_NDELAY on Solaris causes a read
 * with no data to return 0 on a tty or pipe, or -1 with
 * errno EAGAIN on a socket. However 0 is ambiguous since
 * it is also returned for EOF */
int hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int nb = 1;
    return ioctl(s, FIONBIO, &nb);
}

int hcnse_socket_blocking(hcnse_socket_t s)
{
    int nb = 0;
    return ioctl(s, FIONBIO, &nb);
}


#elif (HCNSE_WINDOWS)
int hcnse_tcp_nopush(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int hcnse_tcp_push(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return 0;
}

int hcnse_socket_nonblocking(hcnse_socket_t s)
{
    DWORD nb = 1;
    return ioctlsocket(s, FIONBIO, &nb);
}

int hcnse_socket_blocking(hcnse_socket_t s)
{
    DWORD nb = 0;
    return ioctlsocket(s, FIONBIO, &nb);
}


int hcnse_winsock_init_v22(void)
{
    WSADATA data;
    WORD version_requested = MAKEWORD(2, 2);
    return WSAStartup(version_requested, &data);
}
#endif
