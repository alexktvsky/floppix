#include "xxx_errors.h"
#include "xxx_sockets.h"
#include "xxx_listen.h"


static xxx_err_t init_listen_tcp(listen_unit_t *unit)
{
    struct sockaddr_in ip4_sockaddr;
    unit->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (unit->socket == INVALID_SOCKET) {
        return INIT_SOCKET_ERROR; /* Error of initialization new socket */
    }

    /* Set family of protocol, port and IP adrress */
    ip4_sockaddr.sin_family = AF_INET;
    ip4_sockaddr.sin_port = htons(unit->port);
    if ((ip4_sockaddr.sin_addr.s_addr = inet_addr(unit->ip)) == INADDR_NONE) {
        close_socket(unit->socket);
        return ADDR_ERROR; /* Host IP adrress is not correct */
    }

    /* Prevent sticking of port */
    if (tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }

    /* Link socket and ip4_sockaddr */
    if (bind(unit->socket, (struct sockaddr *) &ip4_sockaddr, sizeof(ip4_sockaddr))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_LEN_QUEUE) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return XXX_OK;
}


#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
static xxx_err_t init_listen_tcp6(listen_unit_t *unit)
{
    struct sockaddr_in6 ip6_sockaddr;
    unit->socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    if (unit->socket == -1) {
        return INIT_SOCKET_ERROR; /* Error of initialization new socket */
    }

    /* Set family of protocol, port and IP adrress */
    ip6_sockaddr.sin6_scope_id = if_nametoindex(unit->netface);
    ip6_sockaddr.sin6_family = AF_INET6;
    ip6_sockaddr.sin6_port = htons(unit->port);
    if (!inet_pton(AF_INET6, unit->ip, (void *) &ip6_sockaddr.sin6_addr.s6_addr)) {
        close_socket(unit->socket);
        return ADDR_ERROR; /* Host IP adrress is not correct */
    }

    /* Prevent sticking of port */
    if (tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }
    
    /* Link socket and ip6_sockaddr */
    if (bind(unit->socket, (struct sockaddr *) &ip6_sockaddr, sizeof(ip6_sockaddr))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_LEN_QUEUE) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return XXX_OK;
}

#elif (SYSTEM_WINDOWS)
static xxx_err_t init_listen_tcp6(listen_unit_t *unit)
{
    (void) unit;
    return IPV6_NOT_SUPPORTED;
}
#endif


xxx_err_t init_listen_sockets(listen_unit_t *listeners)
{
    xxx_err_t stat;
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
        if (stat != XXX_OK) {
            return stat;
        }
    }
    return XXX_OK;
}
