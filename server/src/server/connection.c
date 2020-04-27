#include <stdlib.h>
#include <string.h>

#include "server/connection.h"
#include "server/mempool.h"

#define HCNSE_LISTENER_FLAG   0x1
#define HCNSE_CONNECTION_FLAG 0x2


const char *
hcnse_connection_get_addr(char *buf, struct sockaddr_storage *sockaddr)
{
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    if (getnameinfo((const struct sockaddr *)sockaddr, addr_len,
                            buf, NI_MAXHOST, 0, 0, NI_NUMERICHOST) != 0) {
        return NULL;
    }
    return buf;
}

const char *
hcnse_connection_get_port(char *buf, struct sockaddr_storage *sockaddr)
{
    socklen_t addr_len = sizeof(struct sockaddr_storage);

    if (getnameinfo((const struct sockaddr *)sockaddr, addr_len, NULL, 0,
                    buf, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
        return NULL;
    }
    return buf;
}


hcnse_err_t
hcnse_listener_init_ipv4(hcnse_listener_t *listener, const char *ip,
    const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    hcnse_socket_t fd = HCNSE_INVALID_SOCKET;
    hcnse_list_t *connects = NULL;
    hcnse_err_t err;

    memset(listener, 0, sizeof(hcnse_listener_t));

    err = hcnse_list_create1(&connects);
    if (err != HCNSE_OK) {
        goto failed;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = HCNSE_ERR_NET_GAI;
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == HCNSE_INVALID_SOCKET) {
            err = HCNSE_ERR_NET_SOCKET;
            continue;
        }
        if (hcnse_tcp_nopush(fd) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_TCP_NOPUSH;
            continue;
        }
        if (hcnse_socket_nonblocking(fd) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_TCP_NONBLOCK;
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_BIND;
            continue;
        }
        /* Stop search, we found available address */
        else {
            break;
        }
    }
    if (!rp) {
        goto failed;
    }

    socklen_t addr_len = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
                                                            &addr_len) != 0) {
        err = HCNSE_ERR_NET_GSN;
        goto failed;
    }
    listener->identifier = HCNSE_LISTENER_FLAG;
    listener->fd = fd;
    listener->connects = connects;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(fd);
    }
    if (connects) {
        hcnse_list_destroy(connects);
    }
    return err;
}

hcnse_err_t
hcnse_listener_init_ipv6(hcnse_listener_t *listener, const char *ip,
    const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    hcnse_socket_t fd = HCNSE_INVALID_SOCKET;
    hcnse_list_t *connects = NULL;
    hcnse_err_t err;


    memset(listener, 0, sizeof(hcnse_listener_t));

    err = hcnse_list_create1(&connects);
    if (err != HCNSE_OK) {
        goto failed;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = HCNSE_ERR_NET_GAI;
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == HCNSE_INVALID_SOCKET) {
            err = HCNSE_ERR_NET_SOCKET;
            continue;
        }
        if (hcnse_tcp_nopush(fd) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_TCP_NOPUSH;
            continue;
        }
        if (hcnse_socket_nonblocking(fd) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_TCP_NONBLOCK;
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            hcnse_close_socket(fd);
            err = HCNSE_ERR_NET_BIND;
            continue;
        }
        /* Stop search, we found available address */
        else {
            break;
        }
    }
    if (!rp) {
        goto failed;
    }

    socklen_t addr_len = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
                                                            &addr_len) != 0) {
        err = HCNSE_ERR_NET_GSN;
        goto failed;
    }
    listener->identifier = HCNSE_LISTENER_FLAG;
    listener->fd = fd;
    listener->connects = connects;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(fd);
    }
    if (connects) {
        hcnse_list_destroy(connects);
    }
    return err;
}

hcnse_err_t 
hcnse_listener_start_listen(hcnse_listener_t *listener)
{
    if (listen(listener->fd, HCNSE_MAX_CONNECT_QUEUELEN) == -1) {
        return HCNSE_ERR_NET_LISTEN;
    }
    return HCNSE_OK;
}

void
hcnse_listener_close(hcnse_listener_t *listener)
{
    if (listener->fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(listener->fd);
    }
    listener->fd = HCNSE_INVALID_SOCKET;
}

void
hcnse_listener_cleanup(hcnse_listener_t *listener)
{
    hcnse_listener_close(listener);
    // What we need to do else?
}

hcnse_err_t
hcnse_connection_accept(hcnse_connect_t *connect, hcnse_listener_t *listener)
{
    hcnse_socket_t fd;
    socklen_t addr_len;

    addr_len = sizeof(struct sockaddr_in);
    fd = accept(listener->fd,
                        (struct sockaddr *) &connect->sockaddr,
                                                    &addr_len);
    if (fd == HCNSE_INVALID_SOCKET) {
        return HCNSE_ERR_NET_ACCEPT;
    }
    if (hcnse_socket_nonblocking(fd) == -1) {
        hcnse_close_socket(fd);
        return HCNSE_ERR_NET_TCP_NONBLOCK;
    }
    connect->identifier = HCNSE_CONNECTION_FLAG;
    connect->fd = fd;
    connect->owner = listener;
    connect->want_to_write = 0;
    return HCNSE_OK;
}

void
hcnse_connection_close(hcnse_connect_t *connect)
{
    if (connect->fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(connect->fd);
    }
    connect->fd = HCNSE_INVALID_SOCKET;
}

void
hcnse_connection_cleanup(hcnse_connect_t *connect)
{
    hcnse_connection_close(connect);
}

uint8_t
hcnse_connection_identifier(void *instance)
{
    struct temp {
        int8_t identifier;
    };
    struct temp *p = (struct temp *) instance;
    return p->identifier;
}

bool
hcnse_is_listener(void *instance)
{
    struct temp {
        int8_t identifier;
    };
    struct temp *p = (struct temp *) instance;
    return ((p->identifier) == HCNSE_LISTENER_FLAG);
}

bool
hcnse_is_connection(void *instance)
{
    struct temp {
        int8_t identifier;
    };
    struct temp *p = (struct temp *) instance;
    return ((p->identifier) == HCNSE_CONNECTION_FLAG);
}
