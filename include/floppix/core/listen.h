#ifndef FLOPPIX_CORE_LISTEN_H
#define FLOPPIX_CORE_LISTEN_H

#include "floppix/system/net/sockaddr.h"
#include "floppix/core/forwards.h"
#include "floppix/system/errno.h"
#include "floppix/system/os/portable.h"
#include "floppix/algo/list.h"

/* Some operating systems do not support a value more than 5 */
#define FPX_MAX_CONNECT_QUEUELEN 5
#define FPX_MAX_BIND_ATTEMPTS    3
#define FPX_BIND_ATTEMPT_PAUSE   500

#define FPX_LISTENER_ID          0x00000001
#define fpx_is_listener(x)       ((x->type_id) == FPX_LISTENER_ID)
#define fpx_listener_is_ipv4(x)  fpx_sockaddr_is_ipv4((&(x->sockaddr)))
#define fpx_listener_is_ipv6(x)  fpx_sockaddr_is_ipv6((&(x->sockaddr)))

typedef struct fpx_listener_s fpx_listener_t;

struct fpx_listener_s {
    fpx_bool_t type_id;
    fpx_socket_t fd;
    struct sockaddr_storage sockaddr;
    const char *text_addr;
    const char *text_port;
    fpx_list_node_t list_node;

    /* fpx_list_t *connections; */
    /* fpx_bool_t proto; */
    /* The accept function for this socket */
};

fpx_err_t fpx_listener_init_ipv4(fpx_listener_t *listener, const char *addr,
    const char *port);
fpx_err_t fpx_listener_init_ipv6(fpx_listener_t *listener, const char *addr,
    const char *port);
fpx_err_t fpx_listener_bind(fpx_listener_t *listener);
fpx_err_t fpx_listener_open(fpx_listener_t *listener);
void fpx_listener_close(fpx_listener_t *listener);
void fpx_listener_clear(fpx_listener_t *listener);

const char *fpx_listener_get_addr_text(fpx_listener_t *listener, char *buf,
    fpx_size_t bufsize);
const char *fpx_listener_get_port_text(fpx_listener_t *listener, char *buf,
    fpx_size_t bufsize);

#endif /* FLOPPIX_CORE_LISTEN_H */
