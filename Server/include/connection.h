#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include "sockets.h"


typedef struct listener_s listener_t;
typedef struct connect_s connect_t;

struct listener_s {
    socket_t fd;
    struct sockaddr_storage sockaddr;
    list_t *connects;
};

struct connect_s {
    socket_t fd;
    struct sockaddr_storage sockaddr;
    listener_t *owner;
    bool want_to_write;
};


err_t listener_create_ipv4(listener_t **ls, const char *ip, const char *port);
err_t listener_create_ipv6(listener_t **ls, const char *ip, const char *port);
err_t listener_listen(listener_t *ls);
void listener_close(listener_t *ls);
void listener_clean(listener_t *ls); // delete all connections
void listener_destroy(listener_t *ls);


err_t connection_create(connect_t **cn);
err_t connection_accept(connect_t *cn, listener_t *ls);
void connection_clean(connect_t *cn);
void connection_close(connect_t *cn);
void connection_destroy(connect_t *cn);


const char *get_addr(char *buf, struct sockaddr_storage *sockaddr);
const char *get_port(char *buf, struct sockaddr_storage *sockaddr);

#endif /* INCLUDED_CONNECTION_H */
