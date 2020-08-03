#ifndef INCLUDED_HCNSE_LISTEN_H
#define INCLUDED_HCNSE_LISTEN_H


/* Some operating systems do not support a value more than 5 */
#define HCNSE_MAX_CONNECT_QUEUELEN     5

#define HCNSE_LISTEN_ID                0x00000001
#define hcnse_is_listen(x)             ((x->type_id) == HCNSE_LISTENER_ID)

struct hcnse_listen_s {
    hcnse_flag_t type_id;
    hcnse_socket_t fd;
    struct sockaddr_storage sockaddr;
    char *addr_text;
    char *port_text;
    /* hcnse_list_t *connects; */
};

hcnse_err_t hcnse_listen_init_ipv4(hcnse_listen_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listen_init_ipv6(hcnse_listen_t *listener,
    const char *ip, const char *port);
hcnse_err_t hcnse_listen_open(hcnse_listen_t *listener);
void hcnse_listen_close(hcnse_listen_t *listener);
void hcnse_listen_clean(hcnse_listen_t *listener);

const char *hcnse_listen_get_addr_text(hcnse_listen_t *listener, char *buf,
    size_t bufsize);
const char *hcnse_listen_get_port_text(hcnse_listen_t *listener, char *buf,
    size_t bufsize);



#endif /* INCLUDED_HCNSE_LISTEN_H */
