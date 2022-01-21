#include "floppix/core/listen.h"
#include "floppix/system/memory.h"
#include "floppix/system/net/socket.h"
#include "floppix/system/net/sockaddr.h"
#include "floppix/system/net/sockopt.h"
#include "floppix/system/time.h"
#include "floppix/core/log.h"

fpx_err_t
fpx_listener_init_ipv4(fpx_listener_t *listener, const char *addr,
    const char *port)
{
    fpx_memzero(listener, sizeof(fpx_listener_t));

    listener->text_addr = addr;
    listener->text_port = port;
    listener->type_id = FPX_LISTENER_ID;
    listener->fd = FPX_INVALID_SOCKET;
    listener->sockaddr.ss_family = AF_INET;

    return FPX_OK;
}

fpx_err_t
fpx_listener_init_ipv6(fpx_listener_t *listener, const char *addr,
    const char *port)
{
    fpx_memzero(listener, sizeof(fpx_listener_t));

    listener->text_addr = addr;
    listener->text_port = port;
    listener->type_id = FPX_LISTENER_ID;
    listener->fd = FPX_INVALID_SOCKET;
    listener->sockaddr.ss_family = AF_INET6;

    return FPX_OK;
}

fpx_err_t
fpx_listener_bind(fpx_listener_t *listener)
{
    struct addrinfo hints, *result, *rp;
    const char *addr, *port;
    fpx_socket_t fd;
    fpx_socklen_t addrlen;
    fpx_uint_t tries, done;
    fpx_err_t err;

    done = 0;
    result = NULL;
    fd = FPX_INVALID_SOCKET;
    addr = listener->text_addr;
    port = listener->text_port;

    fpx_memzero(&hints, sizeof(struct addrinfo));

    hints.ai_family = listener->sockaddr.ss_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(addr, port, &hints, &result) != 0) {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_EMERG, err, "getaddrinfo() failed to %s:%s",
            addr, port);
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        err = fpx_socket_init(&fd, rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);
        if (err != FPX_OK) {
            continue;
        }
        err = fpx_socket_nopush(fd);
        if (err != FPX_OK) {
            fpx_socket_close(fd);
            continue;
        }
        err = fpx_socket_nonblocking(fd);
        if (err != FPX_OK) {
            fpx_socket_close(fd);
            continue;
        }

        for (tries = 0; tries < FPX_MAX_BIND_ATTEMPTS; tries++) {
            err = fpx_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != FPX_OK) {
                fpx_log_error1(FPX_LOG_INFO, FPX_OK,
                    "Try again to bind after 500ms");
                fpx_msleep(FPX_BIND_ATTEMPT_PAUSE);
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
            fpx_log_error1(FPX_LOG_ERROR, err,
                "Failed to bind %s:%s after %zu attempts", addr, port, tries);
            fpx_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr, &addrlen) != 0)
    {
        err = fpx_get_socket_errno();
        fpx_log_error1(FPX_LOG_EMERG, err, "getsockname() failed to %s:%s",
            addr, port);
        goto failed;
    }

    listener->fd = fd;

    freeaddrinfo(result);

    return FPX_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != FPX_INVALID_SOCKET) {
        fpx_socket_close(fd);
    }
    return err;
}

fpx_err_t
fpx_listener_open(fpx_listener_t *listener)
{
    fpx_err_t err;

    err = fpx_socket_listen(listener->fd, FPX_MAX_CONNECT_QUEUELEN);
    if (err != FPX_OK) {
        return err;
    }

    return FPX_OK;
}

void
fpx_listener_close(fpx_listener_t *listener)
{
    if (listener->fd != FPX_INVALID_SOCKET) {
        fpx_socket_close(listener->fd);
        listener->fd = FPX_INVALID_SOCKET;
    }
}

void
fpx_listener_clear(fpx_listener_t *listener)
{
    fpx_listener_close(listener);
    fpx_memzero(listener, sizeof(fpx_listener_t));
}

const char *
fpx_listener_get_addr_text(fpx_listener_t *listener, char *buf,
    fpx_size_t bufsize)
{
    return fpx_sockaddr_get_addr_text(
        (const struct sockaddr *) &(listener->sockaddr), buf, bufsize);
}

const char *
fpx_listener_get_port_text(fpx_listener_t *listener, char *buf,
    fpx_size_t bufsize)
{
    return fpx_sockaddr_get_port_text(
        (const struct sockaddr *) &(listener->sockaddr), buf, bufsize);
}
