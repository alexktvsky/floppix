#include "hcnse_portable.h"
#include "hcnse_common.h"


#if (HCNSE_LINUX)

hcnse_err_t
hcnse_socket_nopush(hcnse_socket_t sockfd)
{
    int option;
    hcnse_err_t err;

    option = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int)) == -1)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "setsockopt(%d, SO_REUSEADDR, 1) failed", sockfd);
        return err;
    }
    /*
     * int cork = 1;
     * return setsockopt(sockfd, IPPROTO_TCP, TCP_CORK,
     *                     (const void *) &cork, sizeof(int));
     */
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_push(hcnse_socket_t sockfd)
{
    int option;
    hcnse_err_t err;

    option = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        (const void *) &option, sizeof(int)) == -1)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "setsockopt(%d, SO_REUSEADDR, 0) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_nonblocking(hcnse_socket_t sockfd)
{
    int flags;
    hcnse_err_t err;

    flags = fcntl(sockfd, F_GETFL);

    if (fcntl(sockfd, F_SETFL, flags|O_NONBLOCK) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "fcntl(%d, %d|O_NONBLOCK) failed", sockfd, flags);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_blocking(hcnse_socket_t sockfd)
{
    int flags;
    hcnse_err_t err;

    flags = fcntl(sockfd, F_GETFL);

    if (fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK)) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "fcntl(%d, %d&(~O_NONBLOCK)) failed", sockfd, flags);
        return err;
    }
    return HCNSE_OK;
}

#elif (HCNSE_FREEBSD)
hcnse_err_t
hcnse_socket_nopush(hcnse_socket_t sockfd)
{
    int option;
    hcnse_err_t err;

    option = 1;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &option, sizeof(int)) == -1)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "setsockopt(%d, TCP_NOPUSH, 1) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_push(hcnse_socket_t sockfd)
{
    int option;
    hcnse_err_t err;

    option = 0;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &hcnse_socket_nopush, sizeof(int)) == -1)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "setsockopt(%d, TCP_NOPUSH, 0) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_nonblocking(hcnse_socket_t sockfd)
{
    int nb;
    hcnse_err_t err;

    nb = 1;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctl(%d, FIONBIO, 1) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_blocking(hcnse_socket_t sockfd)
{
    int nb;
    hcnse_err_t err;

    nb = 0;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctl(%d, FIONBIO, 0) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}


#elif (HCNSE_DARWIN)


#elif (HCNSE_SOLARIS)
hcnse_err_t
hcnse_socket_nopush(hcnse_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_push(hcnse_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return HCNSE_OK;
}

/* 
 * Setting FIONBIO or O_NDELAY on Solaris causes a read
 * with no data to return 0 on a tty or pipe, or -1 with
 * errno EAGAIN on a socket. However 0 is ambiguous since
 * it is also returned for EOF
 */
hcnse_err_t
hcnse_socket_nonblocking(hcnse_socket_t sockfd)
{
    int nb;
    hcnse_err_t err;

    nb = 1;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctl(%d, FIONBIO, 1) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_blocking(hcnse_socket_t sockfd)
{
    int nb;
    hcnse_err_t err;

    nb = 0;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctl(%d, FIONBIO, 0) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}


#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_socket_nopush(hcnse_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_push(hcnse_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_nonblocking(hcnse_socket_t sockfd)
{
    DWORD nb;
    hcnse_err_t err;

    nb = 1;

    if (ioctlsocket(sockfd, FIONBIO, &nb) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctlsocket(" HCNSE_FMT_SOCKET_T ", FIONBIO, 1) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_blocking(hcnse_socket_t sockfd)
{
    DWORD nb;
    hcnse_err_t err;

    nb = 0;

    if (ioctlsocket(sockfd, FIONBIO, &nb) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "ioctlsocket(" HCNSE_FMT_SOCKET_T ", FIONBIO, 0) failed", sockfd);
        return err;
    }
    return HCNSE_OK;
}

#endif
