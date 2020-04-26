#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "server/events.h"
#include "os/errno.h" // EAGAIN


hcnse_err_t event_read(hcnse_conf_t *conf, hcnse_connect_t *connect)
{
    (void) conf;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New data from %s:%s\n",
        hcnse_connection_get_addr(str_ip, &connect->sockaddr),
        hcnse_connection_get_port(str_port, &connect->sockaddr));

    return HCNSE_OK;
}
