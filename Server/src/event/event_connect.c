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


err_t event_connect(config_t *conf, listener_t *ls)
{
    err_t err;
    connect_t *new_cn;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New connection %s:%s\n",
        get_addr(str_ip, &ls->sockaddr),
        get_port(str_port, &ls->sockaddr));

    connect_t *cn = (connect_t *) list_first(conf->free_connects);

    /* XXX: If free_connects list is empty */
    if (!cn) {
        err = connection_create(&new_cn);
        if (err != OK) {
            goto failed;
        }
    }
    else {
        new_cn = cn;
    }
    err = connection_accept(new_cn, ls);
    if (err != OK) {
        goto failed;
    }

    err = list_append(ls->connects, new_cn);
    if (err != OK) {
        goto failed;
    }
    return OK;

failed:
    return err;
}
