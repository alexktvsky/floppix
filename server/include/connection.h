#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "syshead.h"
#include "errors.h"
#include "sys_sockets.h"
#include "list.h"


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


err_t listener_init_ipv4(listener_t *listener, const char *ip, const char *port);
err_t listener_init_ipv6(listener_t *listener, const char *ip, const char *port);
err_t listener_start_listen(listener_t *listener);
void listener_close(listener_t *listener);
void listener_clean(listener_t *listener); // delete all connections
void listener_destroy(listener_t *listener);


err_t connection_init(connect_t *connect);
err_t connection_accept(connect_t *connect, listener_t *listener);
void connection_clean(connect_t *connect);
void connection_close(connect_t *connect);
void connection_destroy(connect_t *connect);


const char *get_addr(char *buf, struct sockaddr_storage *sockaddr);
const char *get_port(char *buf, struct sockaddr_storage *sockaddr);

#endif /* INCLUDED_CONNECTION_H */
