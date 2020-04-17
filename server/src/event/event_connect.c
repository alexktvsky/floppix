#include <stdint.h>
#include <stddef.h>

#include "events.h"


err_t event_connect(config_t *conf, listener_t *listener)
{
    (void) conf;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug
    printf("New connection %s:%s\n",
        connection_get_addr(str_ip, &listener->sockaddr),
        connection_get_port(str_port, &listener->sockaddr));

    return OK;
}
