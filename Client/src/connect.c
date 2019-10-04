#include <stddef.h>
#include "error_proc.h"
#include "sockets.h"
#include "connect.h"

status_t init_connect_tcp(client_connect_unit_s *unit) {
    struct sockaddr_in IP4SockAddr;
    unit->socket = socket(AF_INET, SOCK_STREAM,   IPPROTO_TCP);
    /*                           PF_INET6 SOCK_DGRAM     IPPROTO_SCTP *
     *                           PF_UNIX  SOCK_SEQPACKET IPPROTO_UDP  *
     *                                    SOCK_RAW       IPPROTO_DCCP */

    if (unit->socket == -1) {
        CloseSocket(unit->socket);
        return INIT_SOCKET_ERROR;
    }

    /* Set family of protocol, port and IP adrress */
    IP4SockAddr.sin_family = AF_INET;
    IP4SockAddr.sin_port = htons(unit->port);
    if ((IP4SockAddr.sin_addr.s_addr = inet_addr(unit->ip)) == INADDR_NONE) {
        CloseSocket(unit->socket);
        return ADDR_ERROR;
    }

    if (connect(unit->socket, (struct sockaddr *) &IP4SockAddr, sizeof(IP4SockAddr)) == -1) {
        CloseSocket(unit->socket);
        return INIT_CONNECT_ERROR;
    }
    return OK;
}
