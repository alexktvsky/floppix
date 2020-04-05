#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "mempool.h"


const char *get_addr(char *buf, struct sockaddr_storage *sockaddr)
{
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    if (getnameinfo((const struct sockaddr *)sockaddr, addr_len,
                            buf, NI_MAXHOST, 0, 0, NI_NUMERICHOST) != 0) {
        return NULL;
    }
    return buf;
}

const char *get_port(char *buf, struct sockaddr_storage *sockaddr)
{
    socklen_t addr_len = sizeof(struct sockaddr_storage);

    if (getnameinfo((const struct sockaddr *)sockaddr, addr_len, NULL, 0,
                    buf, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
        return NULL;
    }
    return buf;
}


err_t listener_init_ipv4(listener_t *listener, const char *ip, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd = SYS_INVALID_SOCKET;
    list_t *connects = NULL;
    err_t err;

    memset(listener, 0, sizeof(listener_t));

    err = list_create1(&connects);
    if (err != OK) {
        goto failed;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = ERR_NET_GAI;
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == SYS_INVALID_SOCKET) {
            err = ERR_NET_SOCKET;
            continue;
        }
        if (tcp_nopush(fd) == -1) {
            close_socket(fd);
            err = ERR_NET_TCP_NOPUSH;
            continue;
        }
        if (socket_nonblocking(fd) == -1) {
            close_socket(fd);
            err = ERR_NET_TCP_NONBLOCK;
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            close_socket(fd);
            err = ERR_NET_BIND;
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
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr, &addr_len) != 0) {
        err = ERR_NET_GSN;
        goto failed;
    }
    listener->fd = fd;
    listener->connects = connects;
    freeaddrinfo(result);

    return OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != SYS_INVALID_SOCKET) {
        close_socket(fd);
    }
    if (connects) {
        list_destroy(connects);
    }
    return err;
}

err_t listener_init_ipv6(listener_t *listener, const char *ip, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd = SYS_INVALID_SOCKET;
    list_t *connects = NULL;
    err_t err;


    memset(listener, 0, sizeof(listener_t));

    err = list_create1(&connects);
    if (err != OK) {
        goto failed;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = ERR_NET_GAI;
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == SYS_INVALID_SOCKET) {
            err = ERR_NET_SOCKET;
            continue;
        }
        if (tcp_nopush(fd) == -1) {
            close_socket(fd);
            err = ERR_NET_TCP_NOPUSH;
            continue;
        }
        if (socket_nonblocking(fd) == -1) {
            close_socket(fd);
            err = ERR_NET_TCP_NONBLOCK;
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            close_socket(fd);
            err = ERR_NET_BIND;
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
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr, &addr_len) != 0) {
        err = ERR_NET_GSN;
        goto failed;
    }
    listener->fd = fd;
    listener->connects = connects;
    freeaddrinfo(result);

    return OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != SYS_INVALID_SOCKET) {
        close_socket(fd);
    }
    if (connects) {
        list_destroy(connects);
    }
    return err;
}

err_t listener_start_listen(listener_t *listener)
{
    if (listen(listener->fd, MAX_CONNECT_QUEUELEN) == -1) {
        return ERR_NET_LISTEN;
    }
    return OK;
}

void listener_close(listener_t *listener)
{
    if (listener->fd != SYS_INVALID_SOCKET) {
        close_socket(listener->fd);
    }
    listener->fd = SYS_INVALID_SOCKET;
    return;
}

void listener_clean(listener_t *listener)
{
    (void) listener;
    // What we need to do?
    return;
}

void listener_destroy(listener_t *listener)
{
    (void) listener;
    // for (listnode_t *n = list_first(listener->connects); n; n = list_next(n)) {
    //     connection_destroy(list_data(n));
    // }
    // list_destroy(listener->connects);
    // free(listener)
    return;
}

err_t connection_init(connect_t *connect)
{
    memset(connect, 0, sizeof(connect_t));
    return OK;
}


err_t connection_accept(connect_t *connect, listener_t *listener)
{
    socket_t new_fd;

    socklen_t addr_len = sizeof(struct sockaddr_in);
    new_fd = accept(listener->fd,
                        (struct sockaddr *) &connect->sockaddr,
                                                    &addr_len);
    if (new_fd == SYS_INVALID_SOCKET) {
        return ERR_NET_ACCEPT;
    }
    connect->fd = new_fd;
    connect->owner = listener;
    return OK;
}

void connection_close(connect_t *connect)
{
    if (connect->fd != SYS_INVALID_SOCKET) {
        close_socket(connect->fd);
    }
    connect->fd = SYS_INVALID_SOCKET;
    return;
}
