#ifndef FPX_SYSTEM_NET_SOCKADDR_H
#define FPX_SYSTEM_NET_SOCKADDR_H

#include "fpx.system.os.portable.h"

#define FPX_ADDR_INVALID            -1
#define FPX_ADDR_IPV4                0
#define FPX_ADDR_IPV6                1

#define FPX_NI_MAXHOST               NI_MAXHOST
#define FPX_NI_MAXSERV               NI_MAXSERV

#define fpx_sockaddr_is_ipv4(sockaddr) \
    (sockaddr->ss_family == AF_INET)
#define fpx_sockaddr_is_ipv6(sockaddr) \
    (sockaddr->ss_family == AF_INET6)


const char * fpx_sockaddr_get_addr_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);
const char *fpx_sockaddr_get_port_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);

#endif /* FPX_SYSTEM_NET_SOCKADDR_H */
