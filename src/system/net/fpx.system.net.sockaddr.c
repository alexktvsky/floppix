#include "fpx.system.net.sockaddr.h"


const char *
fpx_sockaddr_get_addr_text(const struct sockaddr *sockaddr, char *buf,
    size_t bufsize)
{
    fpx_socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo(sockaddr, addrlen, buf, bufsize, 0, 0,
        NI_NUMERICHOST) != 0)
    {
        return NULL;
    }

    return buf;
}

const char *
fpx_sockaddr_get_port_text(const struct sockaddr *sockaddr, char *buf,
    size_t bufsize)
{
    fpx_socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo(sockaddr, addrlen, NULL, 0, buf, bufsize,
        NI_NUMERICHOST|NI_NUMERICSERV) != 0)
    {
        return NULL;
    }

    return buf;
}
