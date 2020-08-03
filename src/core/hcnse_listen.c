#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t
hcnse_listen_init_ipv4(hcnse_listen_t *listen, const char *ip,
    const char *port)
{
    struct addrinfo hints, *result, *rp;
    hcnse_socket_t fd;
    socklen_t addrlen;
    hcnse_uint_t tries, done;
    hcnse_err_t err;

    done = 0;
    result = NULL;
    fd = HCNSE_INVALID_SOCKET;

    // hcnse_memzero(listen, sizeof(hcnse_listen_t));
    hcnse_memzero(&hints, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;


    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
            "getaddrinfo() failed to %s:%s", ip, port);
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        err = hcnse_socket_init(&fd,
            rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (err != HCNSE_OK) {
            continue;
        }
        err = hcnse_socket_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }

        for (tries = 0; tries < 3; tries++) {
            err = hcnse_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != HCNSE_OK) {
                hcnse_log_error1(HCNSE_LOG_INFO, HCNSE_OK,
                    "Try again to bind after 500ms");
                hcnse_msleep(500);
            }
            else {
                done = 1;
                break;
            }
        }
        /* Stop search, we found available address */
        if (done)
            break;
        else {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "Failed to bind %s:%s after %zu attempts", ip, port, tries);
            hcnse_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listen->sockaddr,
            &addrlen) != 0)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
                "getsockname() failed to %s:%s", ip, port);
        goto failed;
    }
    listen->type_id = HCNSE_LISTEN_ID;
    listen->fd = fd;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_socket_close(fd);
    }
    return err;
}

hcnse_err_t
hcnse_listen_init_ipv6(hcnse_listen_t *listen, const char *ip,
    const char *port)
{
    struct addrinfo hints, *result, *rp;
    hcnse_socket_t fd;
    socklen_t addrlen;
    hcnse_uint_t tries, done;
    hcnse_err_t err;

    done = 0;
    result = NULL;
    fd = HCNSE_INVALID_SOCKET;

    // hcnse_memzero(listen, sizeof(hcnse_listen_t));
    hcnse_memzero(&hints, sizeof(struct addrinfo));

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;


    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
            "getaddrinfo() failed to %s:%s", ip, port);
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        err = hcnse_socket_init(&fd,
            rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (err != HCNSE_OK) {
            continue;
        }
        err = hcnse_socket_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        for (tries = 0; tries < 3; tries++) {
            err = hcnse_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != HCNSE_OK) {
                hcnse_log_error1(HCNSE_LOG_INFO, HCNSE_OK,
                    "Try again to bind after 500ms");
                hcnse_msleep(500);
            }
            else {
                done = 1;
                break;
            }
        }
        /* Stop search, we found available address */
        if (done)
            break;
        else {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "Failed to bind %s:%s after %zu attempts", ip, port, tries);
            hcnse_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listen->sockaddr,
            &addrlen) != 0)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "getsockname() failed to %s:%s", ip, port);
        goto failed;
    }
    listen->type_id = HCNSE_LISTEN_ID;
    listen->fd = fd;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_socket_close(fd);
    }
    return err;
}

hcnse_err_t 
hcnse_listen_open(hcnse_listen_t *listen)
{
    hcnse_err_t err;

    err = hcnse_socket_listen(listen->fd, HCNSE_MAX_CONNECT_QUEUELEN);
    if (err != HCNSE_OK) {
        return err;
    }

    return HCNSE_OK;
}

void
hcnse_listen_close(hcnse_listen_t *listen)
{
    if (listen->fd != HCNSE_INVALID_SOCKET) {
        hcnse_socket_close(listen->fd);
        listen->fd = HCNSE_INVALID_SOCKET;
    }
}

void
hcnse_listen_clean(hcnse_listen_t *listen)
{
    hcnse_listen_close(listen);
    hcnse_memzero(listen, sizeof(hcnse_listen_t));
}


const char *
hcnse_listen_get_addr_text(hcnse_listen_t *listen, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_addr_text(
        (const struct sockaddr *) &(listen->sockaddr), buf, bufsize);
}

const char *
hcnse_listen_get_port_text(hcnse_listen_t *listen, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_port_text(
        (const struct sockaddr *) &(listen->sockaddr), buf, bufsize);
}

