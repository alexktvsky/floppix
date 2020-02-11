#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "syshead.h"
#include "errors.h"
#include "sockets.h"
#include "mempool.h"
#include "connection.h"


static err_t open_tcp_socket(listener_t *listener)
{
    err_t err;
    socket_t new_fd;
    sockaddr_t *new_sockaddr = NULL;

    new_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (new_fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_SOCKET;
        goto failed;
    }

    new_sockaddr = malloc(sizeof(sockaddr_t));
    if (!new_sockaddr) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    new_sockaddr->sockaddr_in.sin_family = AF_INET;
    new_sockaddr->sockaddr_in.sin_port = htons(listener->port);
    new_sockaddr->sockaddr_in.sin_addr.s_addr = inet_addr(listener->ip);
    if (new_sockaddr->sockaddr_in.sin_addr.s_addr == INADDR_NONE) {
        err = ERR_NET_ADDR;
        goto failed;
    }

    if (tcp_nopush(new_fd) == -1) {
        err = ERR_NET_TCP_NOPUSH;
        goto failed;
    }

    if (socket_nonblocking(new_fd) == -1) {
        err = ERR_NET_TCP_NONBLOCK;
        goto failed;
    }


    if (bind(new_fd, (struct sockaddr *) &(new_sockaddr->sockaddr_in),
                                            sizeof(struct sockaddr_in)) == -1) {
        err = ERR_NET_BIND;
        goto failed;
    }

    if (listen(new_fd, MAX_CONNECT_QUEUELEN) == -1) {
        err = ERR_NET_LISTEN;
        goto failed;
    }

    listener->fd = new_fd;
    listener->sockaddr = new_sockaddr;
    return OK;

failed:
    if (new_fd != SYS_INVALID_SOCKET) {
        close_socket(new_fd);
    }
    if (new_sockaddr) {
        free(new_sockaddr);
    }
    return err;
}

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
static err_t open_tcp_socket_ipv6(listener_t *listener)
{
    err_t err;
    socket_t new_fd;
    sockaddr_t *new_sockaddr = NULL;

    new_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (new_fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_SOCKET;
        goto failed;
    }

    new_sockaddr = malloc(sizeof(sockaddr_t));
    if (!new_sockaddr) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    new_sockaddr->sockaddr_in6.sin6_family = AF_INET6;
    new_sockaddr->sockaddr_in6.sin6_port = htons(listener->port);
    if (!inet_pton(AF_INET6, listener->ip,
                (void *) new_sockaddr->sockaddr_in6.sin6_addr.s6_addr)) {
        err = ERR_NET_ADDR;
        goto failed;
    }

    if (tcp_nopush(new_fd) == -1) {
        err = ERR_NET_TCP_NOPUSH;
        goto failed;
    }

    if (socket_nonblocking(new_fd) == -1) {
        err = ERR_NET_TCP_NONBLOCK;
        goto failed;
    }

    if (bind(new_fd, (struct sockaddr *) &(new_sockaddr->sockaddr_in6),
                                        sizeof(struct sockaddr_in6)) == -1) {
        err = ERR_NET_BIND;
        goto failed;
    }

    if (listen(new_fd, MAX_CONNECT_QUEUELEN) == -1) {
        err = ERR_NET_LISTEN;
        goto failed;
    }

    listener->fd = new_fd;
    listener->sockaddr = new_sockaddr;
    return OK;

failed:
    if (new_fd != SYS_INVALID_SOCKET) {
        close_socket(new_fd);
    }
    if (new_sockaddr) {
        free(new_sockaddr);
    }
    return err;
}
#elif (SYSTEM_WINDOWS)
static err_t open_tcp_socket_ipv6(listener_t *listener)
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
            err = open_tcp_socket_ipv6(current);
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


err_t create_and_accept_connection(listener_t *listener)
{
    err_t err;
    socket_t new_fd;
    connect_t *new_conn = NULL;
    sockaddr_t *new_sockaddr = NULL;

    new_conn = malloc(sizeof(connect_t));
    if (!new_conn) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_conn, 0, sizeof(connect_t));

    new_sockaddr = malloc(sizeof(sockaddr_t));
    if (!new_sockaddr) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);
    new_fd = accept(listener->fd,
                            (struct sockaddr *) &(new_sockaddr->sockaddr_in),
                                                            &sockaddr_in_size);
    if (new_fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_ACCEPT;
        goto failed;
    }
    new_conn->fd = new_fd;
    new_conn->sockaddr = new_sockaddr;
    new_conn->owner = listener;
    new_conn->self = new_conn;
    /* XXX: Have not to do it because we use memset()
     * new_conn->next = NULL; */

    /* If connection is first for this listener*/
    if (!listener->connects) {
        listener->connects = new_conn;
        /* XXX: Have not to do it because we use memset()
         * new_conn->prev = NULL; */
    }



    // !!! TODO: SET PREV POINTER TO NODE !!!




    /* Go to the end of list and add new connection */
    else {
        connect_t *temp1 = listener->connects;
        connect_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->next;
        }
        temp2->next = new_conn;
    }
    return OK;

failed:
    if (new_conn) {
        free(new_conn);
    }
    if (new_sockaddr) {
        free(new_sockaddr);
    }
    return err;
}

void destroy_and_close_connection(connect_t *conn)
{
    free(conn->sockaddr);
    free(conn);

    // !!! TODO: REMOVE FROM DOUBLE LINKED LIST !!!

    return;
}

const char *get_connect_ip(connect_t *connect) {
    return inet_ntoa(connect->sockaddr->sockaddr_in.sin_addr);
}

uint16_t get_connect_port(connect_t *connect) {
    return ntohs(connect->sockaddr->sockaddr_in.sin_port);
}
