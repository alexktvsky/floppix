#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include "sockets.h"

#define LIST_MAXFREE_NODES 3 // 10


typedef struct listener_s listener_t;
typedef struct connect_s connect_t;
typedef struct listen_list_s listen_list_t;
typedef struct conn_list_s conn_list_t;

struct listener_s {
    socket_t fd;
    struct sockaddr_storage sockaddr;
    conn_list_t *connects;
    listener_t *next;
    listener_t *prev;
};

struct connect_s {
    socket_t fd;
    struct sockaddr_storage sockaddr;
    listener_t *owner;
    connect_t *next;
    connect_t *prev;
    bool want_to_write;
};

struct listen_list_s {
    size_t size;
    listener_t *head;
    listener_t *tail;
};

struct conn_list_s {
    size_t size;
    connect_t *head;
    connect_t *tail;
    size_t nfree;
    connect_t *free;
    connect_t *free_tail;
};


err_t listen_list_create(listen_list_t **list);
err_t listen_list_append_ipv4(listen_list_t *list, const char *ip, const char *port);
err_t listen_list_append_ipv6(listen_list_t *list, const char *ip, const char *port);
err_t listen_list_remove(listen_list_t *list, listener_t *ls);
void listen_list_clean(listen_list_t *list);
void listen_list_destroy(listen_list_t *list);

err_t listener_listen(listener_t *ls);
void listener_close(listener_t *ls);


err_t conn_list_create(conn_list_t **list);
err_t conn_list_append(conn_list_t *list, listener_t *ls);
err_t conn_list_remove(conn_list_t *list, connect_t *cn);
err_t conn_list_remove_explicit(conn_list_t *list, connect_t *cn);
void conn_list_clean(conn_list_t *list);
void conn_list_destroy(conn_list_t *list);

void connection_close(connect_t *cn);

const char *get_addr(char *buf, struct sockaddr_storage *sockaddr);
const char *get_port(char *buf, struct sockaddr_storage *sockaddr);

#endif /* INCLUDED_CONNECTION_H */
