#include "hcnse.core.listen.h"
#include "hcnse.system.memory.h"
#include "hcnse.system.net.socket.h"
#include "hcnse.system.net.sockaddr.h"
#include "hcnse.system.net.sockopt.h"
#include "hcnse.system.time.h"
#include "hcnse.core.log.h"



hcnse_err_t
hcnse_listener_init_ipv4(hcnse_listener_t *listener, const char *addr,
    const char *port)
{
    hcnse_memzero(listener, sizeof(hcnse_listener_t));

    listener->text_addr = addr;
    listener->text_port = port;
    listener->type_id = HCNSE_LISTENER_ID;
    listener->fd = HCNSE_INVALID_SOCKET;
    listener->sockaddr.ss_family = AF_INET;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_listener_init_ipv6(hcnse_listener_t *listener, const char *addr,
    const char *port)
{
    hcnse_memzero(listener, sizeof(hcnse_listener_t));

    listener->text_addr = addr;
    listener->text_port = port;
    listener->type_id = HCNSE_LISTENER_ID;
    listener->fd = HCNSE_INVALID_SOCKET;
    listener->sockaddr.ss_family = AF_INET6;

    return HCNSE_OK;
}

hcnse_err_t 
hcnse_listener_bind(hcnse_listener_t *listener)
{
    struct addrinfo hints, *result, *rp;
    const char *addr, *port;
    hcnse_socket_t fd;
    hcnse_socklen_t addrlen;
    hcnse_uint_t tries, done;
    hcnse_err_t err;

    done = 0;
    result = NULL;
    fd = HCNSE_INVALID_SOCKET;
    addr = listener->text_addr;
    port = listener->text_port;

    hcnse_memzero(&hints, sizeof(struct addrinfo));

    hints.ai_family = listener->sockaddr.ss_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(addr, port, &hints, &result) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
            "getaddrinfo() failed to %s:%s", addr, port);
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

        for (tries = 0; tries < HCNSE_MAX_BIND_ATTEMPTS; tries++) {
            err = hcnse_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != HCNSE_OK) {
                hcnse_log_error1(HCNSE_LOG_INFO, HCNSE_OK,
                    "Try again to bind after 500ms");
                hcnse_msleep(HCNSE_BIND_ATTEMPT_PAUSE);
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
                "Failed to bind %s:%s after %zu attempts", addr, port, tries);
            hcnse_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
            &addrlen) != 0)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
                "getsockname() failed to %s:%s", addr, port);
        goto failed;
    }

    listener->fd = fd;


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
hcnse_listener_open(hcnse_listener_t *listener)
{
    hcnse_err_t err;

    err = hcnse_socket_listen(listener->fd, HCNSE_MAX_CONNECT_QUEUELEN);
    if (err != HCNSE_OK) {
        return err;
    }

    return HCNSE_OK;
}

void
hcnse_listener_close(hcnse_listener_t *listener)
{
    if (listener->fd != HCNSE_INVALID_SOCKET) {
        hcnse_socket_close(listener->fd);
        listener->fd = HCNSE_INVALID_SOCKET;
    }
}

void
hcnse_listener_clear(hcnse_listener_t *listener)
{
    hcnse_listener_close(listener);
    hcnse_memzero(listener, sizeof(hcnse_listener_t));
}


const char *
hcnse_listener_get_addr_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_addr_text(
        (const struct sockaddr *) &(listener->sockaddr), buf, bufsize);
}

const char *
hcnse_listener_get_port_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_port_text(
        (const struct sockaddr *) &(listener->sockaddr), buf, bufsize);
}
