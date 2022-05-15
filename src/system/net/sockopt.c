#include "floppix/system/net/sockopt.h"
#include "floppix/core/log.h"

#if (FPX_LINUX)

fpx_err_t
fpx_socket_nopush(fpx_socket_t sockfd)
{
    int option;
    fpx_err_t err;

    option = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option,
            sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, SO_REUSEADDR, 1) failed", sockfd);
        return err;
    }
    /*
     * int cork = 1;
     * return setsockopt(sockfd, IPPROTO_TCP, TCP_CORK,
     *                     (const void *) &cork, sizeof(int));
     */
    return FPX_OK;
}

fpx_err_t
fpx_socket_push(fpx_socket_t sockfd)
{
    int option;
    fpx_err_t err;

    option = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option,
            sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, SO_REUSEADDR, 0) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_nonblocking(fpx_socket_t sockfd)
{
    int flags;
    fpx_err_t err;

    flags = fcntl(sockfd, F_GETFL);

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "fcntl(%i, %i|O_NONBLOCK) failed",
            sockfd, flags);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_blocking(fpx_socket_t sockfd)
{
    int flags;
    fpx_err_t err;

    flags = fcntl(sockfd, F_GETFL);

    if (fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK)) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "fcntl(%i, %i&(~O_NONBLOCK)) failed",
            sockfd, flags);
        return err;
    }
    return FPX_OK;
}

#elif (FPX_FREEBSD)
fpx_err_t
fpx_socket_nopush(fpx_socket_t sockfd)
{
    int option;
    fpx_err_t err;

    option = 1;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH, (const void *) &option,
            sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, TCP_NOPUSH, 1) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_push(fpx_socket_t sockfd)
{
    int option;
    fpx_err_t err;

    option = 0;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &fpx_socket_nopush, sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, TCP_NOPUSH, 0) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_nonblocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 1;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 1) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_blocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 0;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 0) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

#elif (FPX_DARWIN)
fpx_err_t
fpx_socket_nopush(fpx_socket_t sockfd)
{
    int option;
    fpx_err_t err;

    option = 1;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH, (const void *) &option,
            sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, TCP_NOPUSH, 1) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_push(fpx_socket_t sockfd)
{
    fpx_err_t err;

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NOPUSH,
            (const void *) &fpx_socket_nopush, sizeof(int))
        == -1)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "setsockopt(%i, TCP_NOPUSH, 0) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_nonblocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 1;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 1) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_blocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 0;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 0) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

#elif (FPX_SOLARIS)
fpx_err_t
fpx_socket_nopush(fpx_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return FPX_OK;
}

fpx_err_t
fpx_socket_push(fpx_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return FPX_OK;
}

/*
 * Setting FIONBIO or O_NDELAY on Solaris causes a read
 * with no data to return 0 on a tty or pipe, or -1 with
 * errno EAGAIN on a socket. However 0 is ambiguous since
 * it is also returned for EOF
 */
fpx_err_t
fpx_socket_nonblocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 1;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 1) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_blocking(fpx_socket_t sockfd)
{
    int nb;
    fpx_err_t err;

    nb = 0;

    if (ioctl(sockfd, FIONBIO, &nb) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ioctl(%i, FIONBIO, 0) failed",
            sockfd);
        return err;
    }
    return FPX_OK;
}

#elif (FPX_WIN32)

fpx_err_t
fpx_socket_nopush(fpx_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return FPX_OK;
}

fpx_err_t
fpx_socket_push(fpx_socket_t sockfd)
{
    /* XXX: do smth */
    (void) sockfd;
    return FPX_OK;
}

fpx_err_t
fpx_socket_nonblocking(fpx_socket_t sockfd)
{
    DWORD nb;
    fpx_err_t err;

    nb = 1;

    if (ioctlsocket(sockfd, FIONBIO, &nb) != 0) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "ioctlsocket(" FPX_FMT_SOCKET_T ", FIONBIO, 1) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_socket_blocking(fpx_socket_t sockfd)
{
    DWORD nb;
    fpx_err_t err;

    nb = 0;

    if (ioctlsocket(sockfd, FIONBIO, &nb) != 0) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "ioctlsocket(" FPX_FMT_SOCKET_T ", FIONBIO, 0) failed", sockfd);
        return err;
    }
    return FPX_OK;
}

#endif
