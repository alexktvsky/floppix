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


err_t listen_list_create(listen_list_t **list)
{
    listen_list_t *new_list = malloc(sizeof(listen_list_t));
    if (!new_list) {
        return ERR_MEM_ALLOC;
    }
    memset(new_list, 0, sizeof(listen_list_t));
    *list = new_list;
    return OK;
}

err_t listen_list_append_ipv4(listen_list_t *list, const char *ip, const char *port)
{
    err_t err;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd;
    listener_t *new_ls = NULL;

    new_ls = malloc(sizeof(listener_t));
    if (!new_ls) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_ls, 0 ,sizeof(listener_t));
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

    /* Append to linked list */

    /* If list is empty */
    if (!list->head) {
        list->head = new_ls;
        new_ls->prev = NULL;
    }
    else {
        list->tail->next = new_ls;
        new_ls->prev = list->tail;
    }
    list->tail = new_ls;

    new_ls->next = NULL;
    list->size += 1;

    freeaddrinfo(result);
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

err_t listen_list_append_ipv6(listen_list_t *list, const char *ip, const char *port)
{
    err_t err;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    socket_t fd;
    listener_t *new_ls = NULL;

    new_ls = malloc(sizeof(listener_t));
    if (!new_ls) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_ls, 0 ,sizeof(listener_t));
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

    /* Append to linked list */

    /* If list is empty */
    if (!list->head) {
        list->head = new_ls;
        new_ls->prev = NULL;
    }
    else {
        list->tail->next = new_ls;
        new_ls->prev = list->tail;
    }
    list->tail = new_ls;

    new_ls->next = NULL;
    list->size += 1;

    freeaddrinfo(result);
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

err_t listen_list_remove(listen_list_t *list, listener_t *ls) {
    listener_t *temp1 = list->head;
    listener_t *temp2 = list->tail;

    /* If first element */
    if (list->head == ls) {
        list->head->next->prev = NULL;
        list->head = list->head->next;
        free(temp1);
        list->size -= 1;
        return OK;
    }
    /* If the last element */
    else if (list->tail == ls) {
        list->tail->prev->next = NULL;
        list->tail = list->tail->prev;
        free(temp2);
        list->size -= 1;
        return OK;
    }
    else {
        temp1 = temp1->next; /* Fisrt element not suitable */
        while (temp1 != ls) {
            temp2 = temp1;
            temp1 = temp1->next;
            if (temp1 == list->tail) { /* Last element not suitable */
                return ERR_FAILED;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        free(temp1);
        list->size -= 1;
        return OK;
    }
}

void listen_list_clean(listen_list_t *list)
{
    listener_t *temp1 = list->head;
    listener_t *temp2;
    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;
        free(temp2);
        list->size -= 1;
    }
    return;
}

void listen_list_destroy(listen_list_t *list)
{
    listen_list_clean(list);
    free(list);
    return;
}


err_t listener_listen(listener_t *ls)
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


static void list_push_free(conn_list_t *list, connect_t *node) {
    /* If list is empty */
    if (!list->free) {
        list->free = node;
        node->prev = NULL;
    }
    else {
        list->free_tail->next = node;
        node->prev = list->free_tail;
    }
    list->free_tail = node;

    node->next = NULL;
    list->nfree += 1;
    return;
}

static connect_t *list_pull_free(conn_list_t *list) {
    connect_t *temp1 = list->free;
    if (!list->free) {
        return NULL;
    }
    if (list->free->next) {
        list->free->next->prev = NULL;
    }
    list->free = list->free->next;
    list->nfree -= 1;
    return temp1;
}


err_t conn_list_create(conn_list_t **list)
{
    conn_list_t *new_list = malloc(sizeof(conn_list_t));
    if (!new_list) {
        return ERR_MEM_ALLOC;
    }
    memset(new_list, 0, sizeof(conn_list_t));
    *list = new_list;
    return OK;
}

err_t conn_list_append(conn_list_t *list, listener_t *ls)
{
    err_t err;
    socket_t new_fd;
    connect_t *new_conn = NULL;
    bool alloc_with_malloc;

    if (list->nfree > 0) {
        new_conn = list_pull_free(list);
        alloc_with_malloc = false;
    }
    else {
        new_conn = malloc(sizeof(listener_t));
        alloc_with_malloc = true;
    }
    if (!new_conn) {
        return ERR_MEM_ALLOC;
    }

    memset(new_conn, 0, sizeof(connect_t));

    socklen_t addr_len = sizeof(struct sockaddr_in);
    new_fd = accept(ls->fd,
                            (struct sockaddr *) &new_conn->sockaddr,
                                                            &addr_len);
    if (new_fd == SYS_INVALID_SOCKET) {
        err = ERR_NET_ACCEPT;
        goto failed;
    }
    new_conn->fd = new_fd;
    new_conn->owner = ls;

    /* Append to the list */
    /* If list is empty */
    if (!list->head) {
        list->head = new_conn;
        new_conn->prev = NULL;
    }
    else {
        list->tail->next = new_conn;
        new_conn->prev = list->tail;
    }
    list->tail = new_conn;

    new_conn->next = NULL;
    list->size += 1;

    return OK;

failed:
    /* TODO: Is it correct? */
    if (new_conn) {
        if (alloc_with_malloc) {
            free(new_conn);
        }
        else {
            if (list->nfree < LIST_MAXFREE_NODES) {
                list_push_free(list, new_conn);
            }
            else {
                conn_list_remove(list, new_conn);
            }
        }
    }
    return err;
}

void connection_close(connect_t *cn)
{
    if (cn->fd != SYS_INVALID_SOCKET) {
        close_socket(cn->fd);
    }
    cn->fd = SYS_INVALID_SOCKET;
    return;
}

err_t conn_list_remove(conn_list_t *list, connect_t *cn)
{
    connect_t *temp1 = list->head;
    connect_t *temp2 = list->tail;

    if (cn->fd != SYS_INVALID_SOCKET) {
        close_socket(cn->fd);
    }

    /* If first element */
    if (list->head == cn) {
        if (list->head->next) {
            list->head->next->prev = NULL;
        }
        list->head = list->head->next;
        if (list->nfree < LIST_MAXFREE_NODES) {
            list_push_free(list, temp1);
        }
        else {
            free(temp1);
        }
        list->size -= 1;
        return EXIT_SUCCESS;
    }
    /* If the last element */
    else if (list->tail == cn) {
        if (list->tail->prev) {
            list->tail->prev->next = NULL;
        }
        list->tail = list->tail->prev;
        if (list->nfree < LIST_MAXFREE_NODES) {
            list_push_free(list, temp2);
        }
        else {
            free(temp2);
        }
        list->size -= 1;
        return EXIT_SUCCESS;
    }
    else {
        temp1 = temp1->next; /* Fisrt element not suitable */
        while (temp1 != cn) {
            temp2 = temp1;
            temp1 = temp1->next;
            if (temp1 == list->tail) { /* Last element not suitable */
                return EXIT_FAILURE;
            }
        }
        temp1->next->prev = temp1->prev;
        temp1->prev->next = temp1->next;
        if (list->nfree < LIST_MAXFREE_NODES) {
            list_push_free(list, temp1);
        }
        else {
            free(temp1);
        }
        list->size -= 1;
        return EXIT_SUCCESS;
    }
}


void conn_list_clean(conn_list_t *list)
{
    connect_t *temp1 = list->head;
    connect_t *temp2;

    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;
        free(temp2);
        list->size -= 1;
    }
    return;
}

void conn_list_destroy(conn_list_t *list)
{
    conn_list_clean(list);
    free(list);
    return;
}


