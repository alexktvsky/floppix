#ifndef INCLUDED_HCNSE_CONNECTION_H
#define INCLUDED_HCNSE_CONNECTION_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_listener_get_addr(listener) \
    hcnse_connection_get_addr((hcnse_connect_t *) listener)
#define hcnse_listener_get_port(listener) \
    hcnse_connection_get_port((hcnse_connect_t *) listener)

struct hcnse_listener_s {
    int8_t identifier;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    // hcnse_list_t *connects;
};

struct hcnse_connect_s {
    int8_t identifier;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    hcnse_listener_t *owner;
    hcnse_pool_t *pool;

    // hcnse_event_t read;
    // hcnse_event_t write;
    // bool ready_to_write;

};

hcnse_err_t hcnse_listener_init_ipv4(hcnse_listener_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listener_init_ipv6(hcnse_listener_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listener_open(hcnse_listener_t *listener);
void hcnse_listener_close(hcnse_listener_t *listener);
void hcnse_listener_clean(hcnse_listener_t *listener);

hcnse_err_t hcnse_connection_init(hcnse_connect_t *connect);
hcnse_err_t hcnse_connection_accept(hcnse_connect_t *connect,
    hcnse_listener_t *listener);
void hcnse_connection_close(hcnse_connect_t *connect);
void hcnse_connection_clean(hcnse_connect_t *connect);
void hcnse_connection_destroy(hcnse_connect_t *connect);

const char *hcnse_connection_get_addr(hcnse_connect_t *connect);
const char *hcnse_connection_get_port(hcnse_connect_t *connect);

bool hcnse_is_listener(void *instance);
bool hcnse_is_connection(void *instance);

#endif /* INCLUDED_HCNSE_CONNECTION_H */
