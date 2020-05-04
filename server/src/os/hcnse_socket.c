#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_LINUX)
hcnse_err_t hcnse_tcp_nopush(hcnse_socket_t s)
{
    int option = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int)) == -1) {
        return hcnse_get_socket_errno();
    }
    // int  cork = 1;
    // return setsockopt(s, IPPROTO_TCP, TCP_CORK,
    //                      (const void *) &cork, sizeof(int));
    return HCNSE_OK;
}

hcnse_err_t hcnse_tcp_push(hcnse_socket_t s)
{
    int option = 0;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int)) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    if (fcntl(s, F_SETFL, flags|O_NONBLOCK) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s)
{
    int flags = fcntl(s, F_GETFL);
    if (fcntl(s, F_SETFL, flags & (~O_NONBLOCK)) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

#elif (HCNSE_FREEBSD)
hcnse_err_t hcnse_tcp_nopush(hcnse_socket_t s)
{
    int hcnse_tcp_nopush = 1;
    if (setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &hcnse_tcp_nopush, sizeof(int)) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_tcp_push(hcnse_socket_t s) {
    int hcnse_tcp_nopush = 0;
    if (setsockopt(s, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &hcnse_tcp_nopush, sizeof(int)) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int nb = 1;
    if (ioctl(s, FIONBIO, &nb) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s)
{
    int nb = 0;
    if (ioctl(s, FIONBIO, &nb) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}


#elif (HCNSE_SOLARIS)
hcnse_err_t hcnse_tcp_nopush(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return HCNSE_OK;
}

hcnse_err_t hcnse_tcp_push(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return HCNSE_OK;
}

/* Setting FIONBIO or O_NDELAY on Solaris causes a read
 * with no data to return 0 on a tty or pipe, or -1 with
 * errno EAGAIN on a socket. However 0 is ambiguous since
 * it is also returned for EOF */
hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s)
{
    int nb = 1;
    if (ioctl(s, FIONBIO, &nb) == -1) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s)
{
    int nb = 0;
    if (ioctl(s, FIONBIO, &nb) == -1) {
    }
    return HCNSE_OK;
}


#elif (HCNSE_WIN32)
hcnse_err_t hcnse_tcp_nopush(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return HCNSE_OK;
}

hcnse_err_t hcnse_tcp_push(hcnse_socket_t s)
{
    /* XXX: do smth */
    (void) s;
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s)
{
    DWORD nb = 1;
    if (ioctlsocket(s, FIONBIO, &nb) != 0) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s)
{
    DWORD nb = 0;
    if (ioctlsocket(s, FIONBIO, &nb) != 0) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_winsock_init_v22(void)
{
    WSADATA data;
    WORD version_requested = MAKEWORD(2, 2);
    if (WSAStartup(version_requested, &data) != 0) {
        return hcnse_get_socket_errno();
    }
    return HCNSE_OK;
}
#endif
