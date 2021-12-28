#ifndef HCNSE_CORE_CONNECTION_H
#define HCNSE_CORE_CONNECTION_H

#include "hcnse.system.errno.h"
#include "hcnse.system.pool.h"
#include "hcnse.core.listen.h"

#define HCNSE_CONNECTION_ID            0x00000002
#define hcnse_is_connection(x)         ((x->type_id) == HCNSE_CONNECTION_ID)

typedef struct hcnse_connect_s hcnse_connect_t;

struct hcnse_connect_s {
    bool type_id;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    char *addr_text;
    char *port_text;
    hcnse_listener_t *owner;
    hcnse_pool_t *pool;

    /* hcnse_event_t read; */
    /* hcnse_event_t write; */
    /* hcnse_uint_t ready_to_write; */
};


hcnse_err_t hcnse_connection_init(hcnse_connect_t *connect);
hcnse_err_t hcnse_connection_accept(hcnse_connect_t *connect,
    hcnse_listener_t *listen);
void hcnse_connection_close(hcnse_connect_t *connect);
void hcnse_connection_clear(hcnse_connect_t *connect);
void hcnse_connection_destroy(hcnse_connect_t *connect);

const char *hcnse_connection_get_addr_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize);
const char *hcnse_connection_get_port_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize);

#endif /* HCNSE_CORE_CONNECTION_H */
