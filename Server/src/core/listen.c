#include "error_proc.h"
#include "sockets.h"
#include "listen.h"


static status_t init_listen_tcp(listen_unit_t *unit) {
    struct sockaddr_in IP4SockAddr;
    unit->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (unit->socket == -1) {
        close_socket(unit->socket);
        return INIT_SOCKET_ERROR; /* Error of initialization new socket */
    }

    /* Set family of protocol, port and IP adrress */
    IP4SockAddr.sin_family = AF_INET;
    IP4SockAddr.sin_port = htons(unit->port);
    if ((IP4SockAddr.sin_addr.s_addr = inet_addr(unit->ip)) == INADDR_NONE) {
        close_socket(unit->socket);
        return ADDR_ERROR; /* Host IP adrress is not correct */
    }

    /* Prevent sticking of port */
    if (tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }

    /* Link socket and IP4SockAddr */
    if (bind(unit->socket, (struct sockaddr *) &IP4SockAddr, sizeof(IP4SockAddr))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_LEN_QUEUE) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return OK;
}


static status_t init_listen_tcp6(listen_unit_t *unit) {
    struct sockaddr_in6 IP6SockAddr;
    unit->socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    if (unit->socket == -1) {
        return INIT_SOCKET_ERROR; /* Error of initialization new socket */
    }

    /* Set family of protocol, port and IP adrress */
    IP6SockAddr.sin6_scope_id = if_nametoindex(unit->interface);
    IP6SockAddr.sin6_family = AF_INET6;
    IP6SockAddr.sin6_port = htons(unit->port);
    if (!inet_pton(AF_INET6, unit->ip, (void *) &IP6SockAddr.sin6_addr.s6_addr)) {
        close_socket(unit->socket);
        return ADDR_ERROR; /* Host IP adrress is not correct */
    }

    /* Prevent sticking of port */
    if (tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }
    
    /* Link socket and IP6SockAddr */
    if (bind(unit->socket, (struct sockaddr *) &IP6SockAddr, sizeof(IP6SockAddr))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_LEN_QUEUE) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return OK;
}


status_t init_listen_sockets(listen_unit_t *listeners) {
    status_t stat;
    listen_unit_t *temp;
    for (temp = listeners; temp; temp = temp->next) {
        switch (temp->protocol) {
            case LISTEN_TCP:
                temp->ip = "0.0.0.0\0";
                stat = init_listen_tcp(temp);
                break;
            case LISTEN_TCP6:
                temp->ip = "::FFFF:0.0.0.0\0";
                stat = init_listen_tcp6(temp);
                break;
        }
        if (stat != OK) {
            return stat;
        }
    }
    return OK;
}
