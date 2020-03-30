#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "events.h"


err_t event_read(config_t *conf, connect_t *connect, listener_t *listener)
{
    (void) conf, listener;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New data from %s:%s\n",
        get_addr(str_ip, &connect->sockaddr),
        get_port(str_port, &connect->sockaddr));

    return OK;
}
