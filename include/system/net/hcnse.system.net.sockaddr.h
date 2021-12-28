#ifndef HCNSE_SYSTEM_NET_SOCKADDR_H
#define HCNSE_SYSTEM_NET_SOCKADDR_H

#include "hcnse.system.os.portable.h"

#define HCNSE_ADDR_INVALID            -1
#define HCNSE_ADDR_IPV4                0
#define HCNSE_ADDR_IPV6                1

#define HCNSE_NI_MAXHOST               NI_MAXHOST
#define HCNSE_NI_MAXSERV               NI_MAXSERV

#define hcnse_sockaddr_is_ipv4(sockaddr) \
    (sockaddr->ss_family == AF_INET)
#define hcnse_sockaddr_is_ipv6(sockaddr) \
    (sockaddr->ss_family == AF_INET6)


const char * hcnse_sockaddr_get_addr_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);
const char *hcnse_sockaddr_get_port_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);

#endif /* HCNSE_SYSTEM_NET_SOCKADDR_H */
