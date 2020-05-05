#ifndef INCLUDED_HCNSE_CONNECTION_H
#define INCLUDED_HCNSE_CONNECTION_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_listener_get_addr hcnse_connection_get_addr
#define hcnse_listener_get_port hcnse_connection_get_port

struct hcnse_listener_s {
    int8_t identifier;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    hcnse_list_t *connects;
};

struct hcnse_connect_s {
    int8_t identifier;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    hcnse_listener_t *owner;
    bool want_to_write;
};

/* XXX: Don't use memory allocation in this functions! */
hcnse_err_t hcnse_listener_init_ipv4(hcnse_listener_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listener_init_ipv6(hcnse_listener_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listener_start_listen(hcnse_listener_t *listener);
void hcnse_listener_close(hcnse_listener_t *listener);
void hcnse_listener_cleanup(hcnse_listener_t *listener);

hcnse_err_t hcnse_connection_accept(hcnse_connect_t *connect,
    hcnse_listener_t *listener);
void hcnse_connection_close(hcnse_connect_t *connect);
void hcnse_connection_cleanup(hcnse_connect_t *connect);

const char *hcnse_connection_get_addr(char *buf,
    struct sockaddr_storage *sockaddr);
const char *hcnse_connection_get_port(char *buf,
    struct sockaddr_storage *sockaddr);

bool hcnse_is_listener(void *instance);
bool hcnse_is_connection(void *instance);


#endif /* INCLUDED_HCNSE_CONNECTION_H */
