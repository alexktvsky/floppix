#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "events.h"
#include "sys_errno.h" // EAGAIN


err_t event_write(config_t *conf, connect_t *connect)
{
    (void) conf;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("Wait data to %s:%s\n",
        connection_get_addr(str_ip, &connect->sockaddr),
        connection_get_port(str_port, &connect->sockaddr));

    return OK;
}
