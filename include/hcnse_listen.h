#ifndef INCLUDED_HCNSE_LISTEN_H
#define INCLUDED_HCNSE_LISTEN_H


/* Some operating systems do not support a value more than 5 */
#define HCNSE_MAX_CONNECT_QUEUELEN     5

#define HCNSE_LISTENER_ID              0x00000001
#define hcnse_is_listener(x)           ((x->type_id) == HCNSE_LISTENER_ID)

struct hcnse_listener_s {
    hcnse_flag_t type_id;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    const char *text_addr;
    const char *text_port;
    /* hcnse_list_t *connects; */

    /* The accept function for this socket */
};

hcnse_err_t hcnse_listener_init_ipv4(hcnse_listener_t *listener,
    const char *addr, const char *port);
hcnse_err_t hcnse_listener_init_ipv6(hcnse_listener_t *listener,
    const char *addr, const char *port);
hcnse_err_t hcnse_listener_open(hcnse_listener_t *listener);
void hcnse_listener_close(hcnse_listener_t *listener);
void hcnse_listener_clean(hcnse_listener_t *listener);

const char *hcnse_listener_get_addr_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize);
const char *hcnse_listener_get_port_text(hcnse_listener_t *listener, char *buf,
    size_t bufsize);

#endif /* INCLUDED_HCNSE_LISTEN_H */
