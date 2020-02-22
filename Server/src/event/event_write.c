#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "sys_files.h"
#include "log.h"
#include "config.h"
#include "cycle.h"
#include "events.h"


err_t event_write(config_t *conf, connect_t *cn, listener_t *ls)
{
    (void) conf, ls;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("Wait data to %s:%s\n",
        get_addr(str_ip, &cn->sockaddr),
        get_port(str_port, &cn->sockaddr));

    return OK;
}
