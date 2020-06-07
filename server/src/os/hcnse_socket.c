#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t
hcnse_socket_init(hcnse_socket_t *new_sockfd, int domain, int type,
    int protocol)
{
    hcnse_socket_t sockfd;
    hcnse_err_t err;

    sockfd = socket(domain, type, protocol);
    if (sockfd == HCNSE_INVALID_SOCKET) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "socket() failed");
        goto failed;
    }

    *new_sockfd = sockfd;

    return HCNSE_OK;

failed:
    return err;
}

hcnse_err_t
hcnse_socket_bind(hcnse_socket_t sockfd, const struct sockaddr *addr,
    socklen_t addrlen)
{
    hcnse_err_t err;

    if (bind(sockfd, addr, addrlen) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "bind() failed to %s:%s",
            hcnse_sockaddr_get_addr_text(addr),
            hcnse_sockaddr_get_port_text(addr));
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_listen(hcnse_socket_t sockfd, int backlog)
{
    hcnse_err_t err;

    if (listen(sockfd, backlog) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "listen(%d, %d) failed",
            sockfd, backlog);
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_accept(hcnse_socket_t *new_sockfd, hcnse_socket_t sockfd,
    struct sockaddr *addr, socklen_t *addrlen)
{
    hcnse_socket_t sockfd1;
    hcnse_err_t err;

    sockfd1 = accept(sockfd, addr, addrlen);
    if (sockfd1 == HCNSE_INVALID_SOCKET) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "accept() failed to %s:%s",
            hcnse_sockaddr_get_addr_text(addr),
            hcnse_sockaddr_get_port_text(addr));
        goto failed;
    }

    *new_sockfd = sockfd1;

    return HCNSE_OK;

failed:
    return err;
}

hcnse_err_t
hcnse_socket_connect(hcnse_socket_t sockfd, const struct sockaddr *addr,
    socklen_t addrlen)
{
    hcnse_err_t err;

    if (connect(sockfd, addr, addrlen) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "connect() failed to %s:%s",
            hcnse_sockaddr_get_addr_text(addr),
            hcnse_sockaddr_get_port_text(addr));
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_socket_shutdown(hcnse_socket_t sockfd, int how)
{
    hcnse_err_t err;

    if (shutdown(sockfd, how) == -1) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "shutdown(%d, %d) failed",
            sockfd, how);
        return err;
    }
    return HCNSE_OK;
}

#if (HCNSE_WIN32)
hcnse_err_t
hcnse_winsock_init_v22(void)
{
    WSADATA data;
    WORD version_requested;
    hcnse_err_t err;

    version_requested = MAKEWORD(2, 2);

    if (WSAStartup(version_requested, &data) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err, "WSAStartup() failed");
        return err;
    }
    return HCNSE_OK;
}
#endif
