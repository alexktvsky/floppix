#ifndef FLOPPIX_CORE_CONNECTION_H
#define FLOPPIX_CORE_CONNECTION_H

#include "floppix/system/errno.h"
#include "floppix/system/pool.h"
#include "floppix/algo/list.h"
#include "floppix/core/listen.h"

#define FPX_CONNECTION_ID    0x00000002
#define fpx_is_connection(x) ((x->type_id) == FPX_CONNECTION_ID)

typedef struct fpx_connect_s fpx_connect_t;

struct fpx_connect_s {
    bool type_id;
    fpx_socket_t fd;
    struct sockaddr_storage sockaddr;
    char *addr_text;
    char *port_text;
    fpx_listener_t *owner;
    fpx_pool_t *pool;
    fpx_list_node_t list_node;

    /* fpx_event_t read; */
    /* fpx_event_t write; */
    /* bool ready_to_write; */
};

fpx_err_t fpx_connection_init(fpx_connect_t *connect);
fpx_err_t fpx_connection_accept(fpx_connect_t *connect, fpx_listener_t *listen);
void fpx_connection_close(fpx_connect_t *connect);
void fpx_connection_clear(fpx_connect_t *connect);
void fpx_connection_destroy(fpx_connect_t *connect);

const char *fpx_connection_get_addr_text(fpx_connect_t *connect, char *buf,
    size_t bufsize);
const char *fpx_connection_get_port_text(fpx_connect_t *connect, char *buf,
    size_t bufsize);

#endif /* FLOPPIX_CORE_CONNECTION_H */
