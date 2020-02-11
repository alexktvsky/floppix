#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include "sockets.h"


typedef struct listener_s listener_t;
typedef struct connect_s connect_t;

typedef union {
    struct sockaddr sockaddr;
    struct sockaddr_in sockaddr_in;
    struct sockaddr_in6 sockaddr_in6;
} sockaddr_t;


struct listener_s {
    socket_t fd;
    sockaddr_t *sockaddr;
    char *ip;
    uint16_t port;
    bool is_ipv6;
    connect_t *connects; /* Linked list of input connections */
    listener_t *next;
};

struct connect_s {
    socket_t fd;
    sockaddr_t *sockaddr;
    listener_t *owner;
    connect_t *self;
    connect_t *next;
    connect_t *prev;
    bool want_to_write;
};


err_t open_listening_sockets(listener_t *listeners);
void close_listening_sockets(listener_t *listeners);

err_t create_and_accept_connection(listener_t *listener);
void destroy_and_close_connection(connect_t *conn);


const char *get_connect_ip(connect_t *connect);
uint16_t get_connect_port(connect_t *connect);


#endif /* INCLUDED_CONNECTION_H */
