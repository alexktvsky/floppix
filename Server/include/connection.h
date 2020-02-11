#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include "sockets.h"


typedef union {
    struct sockaddr sockaddr;
    struct sockaddr_in sockaddr_in;
    struct sockaddr_in6 sockaddr_in6;
} sockaddr_t;



typedef struct listener_s listener_t;

struct listener_s {
    socket_t fd;
    sockaddr_t *sockaddr;
    char *ip;
    uint16_t port;
    bool is_ipv6;
    listener_t *next;
};


// typedef struct connection_s {
// } connection_t;

err_t open_listening_sockets(listener_t *listeners);
void close_listening_sockets(listener_t *listeners);

#endif /* INCLUDED_CONNECTION_H */
