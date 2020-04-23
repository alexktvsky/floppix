#ifndef INCLUDED_CONNECTION_H
#define INCLUDED_CONNECTION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "syshead.h"
#include "errors.h"
#include "sys_sockets.h"
#include "list.h"

#define IS_LISTENER 0x1
#define IS_CONNECTION 0x2

#define listener_get_addr connection_get_addr
#define listener_get_port connection_get_port


typedef struct listener_s listener_t;
typedef struct connect_s connect_t;

struct listener_s {
    int8_t identifier;
    sys_socket_t fd;
    struct sockaddr_storage sockaddr;
    list_t *connects;
};

struct connect_s {
    int8_t identifier;
    sys_socket_t fd;
    struct sockaddr_storage sockaddr;
    listener_t *owner;
    bool want_to_write;
};

/* XXX: Don't use memory allocation in this functions! */
err_t listener_init_ipv4(listener_t *listener, const char *ip, const char *port);
err_t listener_init_ipv6(listener_t *listener, const char *ip, const char *port);
err_t listener_start_listen(listener_t *listener);
void listener_close(listener_t *listener);
void listener_cleanup(listener_t *listener);

err_t connection_accept(connect_t *connect, listener_t *listener);
void connection_close(connect_t *connect);
void connection_cleanup(connect_t *connect);

const char *connection_get_addr(char *buf, struct sockaddr_storage *sockaddr);
const char *connection_get_port(char *buf, struct sockaddr_storage *sockaddr);

uint8_t connection_identifier(void *instance);

#endif /* INCLUDED_CONNECTION_H */
