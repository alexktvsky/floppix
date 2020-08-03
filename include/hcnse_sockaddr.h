#ifndef INCLUDED_HCNSE_SOCKADDR_H
#define INCLUDED_HCNSE_SOCKADDR_H


#define HCNSE_MAX_ADDR_LEN             NI_MAXHOST
#define HCNSE_MAX_PORT_LEN             NI_MAXSERV


const char * hcnse_sockaddr_get_addr_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);
const char *hcnse_sockaddr_get_port_text(const struct sockaddr *sockaddr,
    char *buf, size_t bufsize);

#endif /* INCLUDED_HCNSE_SOCKADDR_H */
