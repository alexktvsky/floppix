#include "errors.h"
#include "sockets.h"
#include "listen.h"


static hcnse_err_t init_listen_tcp(listen_unit_t *unit)
{
    unit->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (unit->socket == INVALID_SOCKET) {
        return INIT_SOCKET_ERROR; /* Error of initialization new socket */
    }

    /* Set family of protocol, port and IP adrress */
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(unit->port);
    sockaddr_in.sin_addr.s_addr = inet_addr(unit->ip)
    
    if (sockaddr_in.sin_addr.s_addr == INADDR_NONE) {
        close_socket(unit->socket);
        return ADDR_ERROR; /* Host IP adrress is not correct */
    }

    /* Prevent sticking of port */
    if (hcnse_tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }

    /* Link socket and sockaddr_in */
    if (bind(unit->socket, (struct sockaddr *) &sockaddr_in, sizeof(sockaddr_in))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_CONNECT_QUEUELEN) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return HCNSE_OK;
}


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
static hcnse_err_t init_listen_tcp6(listen_unit_t *unit)
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
    if (hcnse_tcp_nopush(unit->socket) == -1) {
        return SETSOCKOPT_ERROR; /* Error of set options on sockets */
    }
    
    /* Link socket and ip6_sockaddr */
    if (bind(unit->socket, (struct sockaddr *) &ip6_sockaddr, sizeof(ip6_sockaddr))) {
        close_socket(unit->socket);
        return BIND_ERROR; /* Error of binding */
    }

    /* Switch to listening state */
    if (listen(unit->socket, MAX_CONNECT_QUEUELEN) == -1) {
        close_socket(unit->socket);
        return INIT_LISTEN_ERROR; /* Error of initialization listening socket */
    }
    return HCNSE_OK;
}

#elif (HCNSE_WIN32)
static hcnse_err_t init_listen_tcp6(listen_unit_t *unit)
{
    (void) unit;
    return IPV6_NOT_SUPPORTED;
}
#endif


hcnse_err_t init_listen_sockets(listen_unit_t *listeners)
{
    hcnse_err_t stat;
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
        if (stat != HCNSE_OK) {
            return stat;
        }
    }
    return HCNSE_OK;
}
