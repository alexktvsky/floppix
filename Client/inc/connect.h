#ifndef CONNECT_H
#define CONNECT_H

typedef struct {
    char *ip;
    int port;
    socket_t socket;
} client_connect_unit_s;


status_t init_connect_tcp(client_connect_unit_s *unit);

#endif /* CONNECT_H */
