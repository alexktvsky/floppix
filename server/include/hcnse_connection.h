#ifndef INCLUDED_HCNSE_CONNECTION_H
#define INCLUDED_HCNSE_CONNECTION_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Some operating systems do not support a value more than 5 */
#define HCNSE_MAX_CONNECT_QUEUELEN     5

#define HCNSE_MAX_ADDR_LEN             NI_MAXHOST
#define HCNSE_MAX_PORT_LEN             NI_MAXSERV

#define HCNSE_LISTENER_ID              0x00000001
#define HCNSE_CONNECTION_ID            0x00000002

#define hcnse_is_listener(x)           ((x->type_id) == HCNSE_LISTENER_ID)
#define hcnse_is_connection(x)         ((x->type_id) == HCNSE_CONNECTION_ID)


struct hcnse_listener_s {
    hcnse_flag_t type_id;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    /* hcnse_list_t *connects; */
};

struct hcnse_connect_s {
    hcnse_flag_t type_id;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    hcnse_listener_t *owner;
    hcnse_pool_t *pool;

    /* hcnse_event_t read; */
    /* hcnse_event_t write; */
    /* hcnse_uint_t ready_to_write; */
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

const char * hcnse_sockaddr_get_addr_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);
const char *hcnse_sockaddr_get_port_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);
const char *hcnse_listener_get_addr_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize);
const char *hcnse_listener_get_port_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize);
const char *hcnse_connection_get_addr_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize);
const char *hcnse_connection_get_port_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize);

#endif /* INCLUDED_HCNSE_CONNECTION_H */
