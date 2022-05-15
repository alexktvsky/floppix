#include "floppix/system/net/socket.h"
#include "floppix/system/net/sockaddr.h"
#include "floppix/core/log.h"

fpx_err_t
fpx_socket_init(fpx_socket_t *new_sockfd, int domain, int type, int protocol)
{
    fpx_socket_t sockfd;
    fpx_err_t err;

    sockfd = socket(domain, type, protocol);
    if (sockfd == FPX_INVALID_SOCKET) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "socket() failed");
        goto failed;
    }

    *new_sockfd = sockfd;

    return FPX_OK;

failed:
    return err;
}

fpx_err_t
fpx_socket_bind(fpx_socket_t sockfd, const struct sockaddr *addr,
    fpx_socklen_t addrlen)
{
    char text_addr[FPX_NI_MAXHOST];
    char text_port[FPX_NI_MAXSERV];
    fpx_err_t err;

    if (bind(sockfd, addr, addrlen) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "bind() failed to %s:%s",
            fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
            fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV));
        return err;
    }

    fpx_log_debug1(FPX_OK, "Bind socket " FPX_FMT_SOCKET_T " to %s:%s", sockfd,
        fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
        fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV));

    return FPX_OK;
}

fpx_err_t
fpx_socket_listen(fpx_socket_t sockfd, int backlog)
{
    fpx_err_t err;

    if (listen(sockfd, backlog) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "listen(%i, %i) failed", sockfd,
            backlog);
        return err;
    }

    fpx_log_debug1(FPX_OK, "Start listening socket " FPX_FMT_SOCKET_T, sockfd);

    return FPX_OK;
}

fpx_err_t
fpx_socket_accept(fpx_socket_t *new_sockfd, fpx_socket_t sockfd,
    struct sockaddr *addr, fpx_socklen_t *addrlen)
{
    char text_addr[FPX_NI_MAXHOST];
    char text_port[FPX_NI_MAXSERV];
    fpx_socket_t sockfd1;
    fpx_err_t err;

    sockfd1 = accept(sockfd, addr, addrlen);
    if (sockfd1 == FPX_INVALID_SOCKET) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "accept() failed to %s:%s",
            fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
            fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV));
        goto failed;
    }

    *new_sockfd = sockfd1;

    fpx_log_debug1(FPX_OK,
        "Accept socket " FPX_FMT_SOCKET_T " from %s:%s to %i", sockfd1,
        fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
        fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV), sockfd);

    return FPX_OK;

failed:
    return err;
}

fpx_err_t
fpx_socket_connect(fpx_socket_t sockfd, const struct sockaddr *addr,
    fpx_socklen_t addrlen)
{
    char text_addr[FPX_NI_MAXHOST];
    char text_port[FPX_NI_MAXSERV];
    fpx_err_t err;

    if (connect(sockfd, addr, addrlen) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "connect() failed to %s:%s",
            fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
            fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV));
        return err;
    }

    fpx_log_debug1(FPX_OK, "Connect socket " FPX_FMT_SOCKET_T " to %s:%s",
        sockfd, fpx_sockaddr_get_addr_text(addr, text_addr, FPX_NI_MAXHOST),
        fpx_sockaddr_get_port_text(addr, text_port, FPX_NI_MAXSERV));

    return FPX_OK;
}

fpx_err_t
fpx_socket_shutdown(fpx_socket_t sockfd, int how)
{
    fpx_err_t err;

    if (shutdown(sockfd, how) == -1) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "shutdown(%i, %i) failed", sockfd,
            how);
        return err;
    }

    fpx_log_debug1(FPX_OK, "Shutdown socket " FPX_FMT_SOCKET_T, sockfd);

    return FPX_OK;
}

#if (FPX_POSIX)

void
fpx_socket_close(fpx_socket_t sockfd)
{
    close(sockfd);
    fpx_log_debug1(FPX_OK, "Close socket " FPX_FMT_SOCKET_T, sockfd);
}

#elif (FPX_WIN32)

void
fpx_socket_close(fpx_socket_t sockfd)
{
    closesocket(sockfd);
    fpx_log_debug1(FPX_OK, "Close socket " FPX_FMT_SOCKET_T, sockfd);
}

fpx_err_t
fpx_winsock_init_v22(void)
{
    WSADATA data;
    WORD version_requested;
    fpx_err_t err;

    version_requested = MAKEWORD(2, 2);

    if (WSAStartup(version_requested, &data) != 0) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_EMERG, err, "WSAStartup() failed");
        return err;
    }
    return FPX_OK;
}

#endif
