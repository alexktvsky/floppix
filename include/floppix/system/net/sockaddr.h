#ifndef FLOPPIX_SYSTEM_NET_SOCKADDR_H
#define FLOPPIX_SYSTEM_NET_SOCKADDR_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"

#define FPX_ADDR_INVALID               -1
#define FPX_ADDR_IPV4                  0
#define FPX_ADDR_IPV6                  1

#define FPX_NI_MAXHOST                 NI_MAXHOST
#define FPX_NI_MAXSERV                 NI_MAXSERV

#define fpx_sockaddr_is_ipv4(sockaddr) (sockaddr->ss_family == AF_INET)
#define fpx_sockaddr_is_ipv6(sockaddr) (sockaddr->ss_family == AF_INET6)

const char *fpx_sockaddr_get_addr_text(const struct sockaddr *sockaddr,
    char *buf, fpx_size_t bufsize);
const char *fpx_sockaddr_get_port_text(const struct sockaddr *sockaddr,
    char *buf, fpx_size_t bufsize);

#endif /* FLOPPIX_SYSTEM_NET_SOCKADDR_H */
