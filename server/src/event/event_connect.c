#include <stdint.h>
#include <stddef.h>

#include "server/events.h"


hcnse_err_t event_connect(hcnse_conf_t *conf, hcnse_listener_t *listener)
{
    (void) conf;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug
    printf("New connection %s:%s\n",
        hcnse_connection_get_addr(str_ip, &listener->sockaddr),
        hcnse_connection_get_port(str_port, &listener->sockaddr));

    return HCNSE_OK;
}
