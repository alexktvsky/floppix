#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "sockets.h"
#include "connection.h"


static err_t open_tcp_socket(listener_t *listener)
{
    err_t err;
    sockaddr_t *sockaddr = NULL;

    listener->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener->fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_SOCKET;
        goto failed;
    }

    sockaddr = malloc(sizeof(sockaddr_t));
    if (!sockaddr) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    sockaddr->sockaddr_in.sin_family = AF_INET;
    sockaddr->sockaddr_in.sin_port = htons(listener->port);
    sockaddr->sockaddr_in.sin_addr.s_addr = inet_addr(listener->ip);
    if (sockaddr->sockaddr_in.sin_addr.s_addr == INADDR_NONE) {
        err = ERR_NET_ADDR;
        goto failed;
    }

    if (tcp_nopush(listener->fd) == -1) {
        err = ERR_NET_TCP_NOPUSH;
        goto failed;
    }

    if (socket_nonblocking(listener->fd) == -1) {
        err = ERR_NET_TCP_NONBLOCK;
        goto failed;
    }


    if (bind(listener->fd, (struct sockaddr *)&(sockaddr->sockaddr_in),
                                            sizeof(struct sockaddr_in)) == -1) {
        err = ERR_NET_BIND;
        goto failed;
    }

    if (listen(listener->fd, MAX_CONNECT_QUEUELEN) == -1) {
        err = ERR_NET_LISTEN;
        goto failed;
    }
    listener->sockaddr = sockaddr;
    return OK;

failed:
    if (listener->fd != SYS_INVALID_SOCKET) {
        close_socket(listener->fd);
    }
    if (sockaddr) {
        free(sockaddr);
    }
    return err;
}

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
static err_t open_tcp6_socket(listener_t *listener)
{
    err_t err;
    sockaddr_t *sockaddr = NULL;

    listener->fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (listener->fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_SOCKET;
        goto failed;
    }

    sockaddr = malloc(sizeof(sockaddr_t));
    if (!sockaddr) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    sockaddr->sockaddr_in6.sin6_family = AF_INET6;
    sockaddr->sockaddr_in6.sin6_port = htons(listener->port);
    if (!inet_pton(AF_INET6, listener->ip,
                (void *) sockaddr->sockaddr_in6.sin6_addr.s6_addr)) {
        err = ERR_NET_ADDR;
        goto failed;
    }

    if (tcp_nopush(listener->fd) == -1) {
        err = ERR_NET_TCP_NOPUSH;
        goto failed;
    }

    if (socket_nonblocking(listener->fd) == -1) {
        err = ERR_NET_TCP_NONBLOCK;
        goto failed;
    }

    if (bind(listener->fd, (struct sockaddr *)&(sockaddr->sockaddr_in6),
                                        sizeof(struct sockaddr_in6)) == -1) {
        err = ERR_NET_BIND;
        goto failed;
    }

    if (listen(listener->fd, MAX_CONNECT_QUEUELEN) == -1) {
        err = ERR_NET_LISTEN;
        goto failed;
    }
    listener->sockaddr = sockaddr;
    return OK;

failed:
    if (listener->fd != SYS_INVALID_SOCKET) {
        close_socket(listener->fd);
    }
    if (sockaddr) {
        free(sockaddr);
    }
    return err;
}
#elif (SYSTEM_WINDOWS)
static err_t open_tcp6_socket(listener_t *listener)
{
    (void) listener;
    return ERR_NET_IPV6;
}
#endif


err_t open_listening_sockets(listener_t *listeners)
{
    err_t err;
    listener_t *current = listeners;
    while (current) {
        if (current->is_ipv6) {
            err = open_tcp6_socket(current);
        }
        else {
            err = open_tcp_socket(current);
        }
        if (err != OK) {
            /* If smth happens wrong close already opened sockets */
            close_listening_sockets(listeners);
            return err;
        }
        current = current->next;
    }
    return OK;
}


static void close_tcp_socket(listener_t *listener)
{
    close_socket(listener->fd);
    if (listener->sockaddr) {
        free(listener->sockaddr);
    }
    return;
}


void close_listening_sockets(listener_t *listeners)
{
    while (listeners) {
        close_tcp_socket(listeners);
        listeners = listeners->next;
    }
    return;
}

