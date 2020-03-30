#include <stdint.h>
#include <stddef.h>

#include "events.h"


err_t event_connect(config_t *conf, listener_t *listener)
{
    err_t err;
    connect_t *connect;
    connect_t *new_connect;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New connection %s:%s\n",
        get_addr(str_ip, &listener->sockaddr),
        get_port(str_port, &listener->sockaddr));

    connect = (connect_t *) list_first(conf->free_connects);

    /* XXX: If free_connects list is empty */
    if (!connect) {
        err = connection_create(&new_connect);
        if (err != OK) {
            goto failed;
        }
    }
    else {
        new_connect = connect;
    }
    err = connection_accept(new_connect, listener);
    if (err != OK) {
        goto failed;
    }

    err = list_append(listener->connects, new_connect);
    if (err != OK) {
        goto failed;
    }
    return OK;

failed:
    return err;
}
