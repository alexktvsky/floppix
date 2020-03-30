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


err_t listener_create_ipv4(listener_t **ls, const char *ip, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd = SYS_INVALID_SOCKET;
    listener_t *new_ls = NULL;
    list_t *new_connects = NULL;
    err_t err;

    new_ls = malloc(sizeof(listener_t));
    if (!new_ls) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_ls, 0, sizeof(listener_t));

    err = list_create(&new_connects);
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
    if (getsockname(fd, (struct sockaddr *) &new_ls->sockaddr, &addr_len) != 0) {
        err = ERR_NET_GSN;
        goto failed;
    }
    new_ls->fd = fd;
    new_ls->connects = new_connects;
    freeaddrinfo(result);

    *ls = new_ls;
    return OK;

failed:
    if (new_ls) {
        free(new_ls);
    }
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != SYS_INVALID_SOCKET) {
        close_socket(fd);
    }
    if (new_connects) {
        list_destroy(new_connects);
    }
    return err;
}

err_t listener_create_ipv6(listener_t **ls, const char *ip, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd = SYS_INVALID_SOCKET;
    listener_t *new_ls = NULL;
    list_t *new_connects = NULL;
    list_t *new_free_connects = NULL;
    err_t err;

    new_ls = malloc(sizeof(listener_t));
    if (!new_ls) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_ls, 0, sizeof(listener_t));

    err = list_create(&new_connects);
    if (err != OK) {
        goto failed;
    }

    err = list_create(&new_free_connects);
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
    if (getsockname(fd, (struct sockaddr *) &new_ls->sockaddr, &addr_len) != 0) {
        err = ERR_NET_GSN;
        goto failed;
    }
    new_ls->fd = fd;
    new_ls->connects = new_connects;
    freeaddrinfo(result);

    *ls = new_ls;
    return OK;

failed:
    if (new_ls) {
        free(new_ls);
    }
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != SYS_INVALID_SOCKET) {
        close_socket(fd);
    }
    return err;
}

err_t listener_start_listen(listener_t *ls)
{
    if (listen(ls->fd, MAX_CONNECT_QUEUELEN) == -1) {
        return ERR_NET_LISTEN;
    }
    return OK;
}

void listener_close(listener_t *ls)
{
    if (ls->fd != SYS_INVALID_SOCKET) {
        close_socket(ls->fd);
    }
    ls->fd = SYS_INVALID_SOCKET;
    return;
}

void listener_clean(listener_t *ls)
{
    (void) ls;
    // What we need to do?
    return;
}

void listener_destroy(listener_t *ls)
{
    (void) ls;
    // for (listnode_t *n = list_first(ls->connects); n; n = list_next(n)) {
    //     connection_destroy(list_data(n));
    // }
    // list_destroy(ls->connects);
    // free(ls)
    return;
}

err_t connection_create(connect_t **cn)
{
    connect_t *new_cn = NULL;

    new_cn = malloc(sizeof(connect_t));
    if (!new_cn) {
        return ERR_MEM_ALLOC;
    }
    memset(new_cn, 0, sizeof(connect_t));
    *cn = new_cn;
    return OK;
}


err_t connection_accept(connect_t *cn, listener_t *ls)
{
    socket_t new_fd;

    socklen_t addr_len = sizeof(struct sockaddr_in);
    new_fd = accept(ls->fd,
                        (struct sockaddr *) &cn->sockaddr,
                                                    &addr_len);
    if (new_fd == SYS_INVALID_SOCKET) {
        return ERR_NET_ACCEPT;
    }
    cn->fd = new_fd;
    cn->owner = ls;
    return OK;
}

void connection_close(connect_t *cn)
{
    if (cn->fd != SYS_INVALID_SOCKET) {
        close_socket(cn->fd);
    }
    cn->fd = SYS_INVALID_SOCKET;
    return;
}

