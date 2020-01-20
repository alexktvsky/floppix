#ifndef XXX_LISTEN_H
#define XXX_LISTEN_H

/* Some operating systems do not support a value more than 5 */
#define MAX_LEN_QUEUE  5

#define LISTEN_TCP     0
#define LISTEN_TCP6    1

typedef struct listen_unit_s listen_unit_t;

struct listen_unit_s {
    char *ip;
    int port;
    char *netface;
    int protocol;
    socket_t socket;
    listen_unit_t *next;
};


status_t init_listen_sockets(listen_unit_t *listeners);

#endif /* XXX_LISTEN_H */
