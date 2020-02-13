#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "syshead.h"

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#endif

#include "errors.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "files.h"
#include "syslog.h"
#include "config.h"
#include "cycle.h"



socket_t select_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    socket_t fd;
    listener_t *ls;
    connect_t *cn;
    listnode_t *i;
    listnode_t *j;
    socket_t fdmax = 0;

    FD_ZERO(rfds);
    FD_ZERO(wfds);

    for (i = list_first(conf->listeners); i; i = list_next(i)) {
        /* Add listening sockets to read array */
        ls = (listener_t *)list_data(i);
        fd = ls->fd;
        FD_SET(fd, rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        j = list_first(((listener_t *)list_data(i))->connects);
        for ( ; j; j = list_next(j)) {
            /* Add clients sockets to read array */
            cn = (connect_t *)list_data(j);
            fd = cn->fd;
            FD_SET(fd, rfds);
            if (fdmax < fd) {
                fdmax = fd;
            }

            /* Add clients sockets to write array */
            if (!cn->want_to_write) {
                continue;
            }
            fd = cn->fd;
            FD_SET(fd, wfds);
            if (fdmax < fd) {
                fdmax = fd;
            }
        }
    }
    return fdmax;
}



err_t handle_new_connection(config_t *conf, connect_t *cn, listener_t *ls)
{
    err_t err;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New connection %s:%s\n",
        get_addr(str_ip, &ls->sockaddr),
        get_port(str_port, &ls->sockaddr));

    listnode_t *temp = list_first(conf->free_connects);

    /* XXX: If free_connects list is empty */
    if (!temp) {
        err = connection_create(&cn);
        if (err != OK) {
            goto failed;
        }
    }
    else {
        cn = list_data(temp);
    }
    err = connection_accept(cn, ls);
    if (err != OK) {
        goto failed;
    }

    err = list_append(ls->connects, cn);
    if (err != OK) {
        goto failed;
    }
    return OK;

failed:
    return err;
}

err_t handle_input_data(config_t *conf, connect_t *cn, listener_t *ls)
{
    (void) conf, ls;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("New data from %s:%s\n",
        get_addr(str_ip, &cn->sockaddr),
        get_port(str_port, &cn->sockaddr));

    return OK;
}

err_t handle_output_data(config_t *conf, connect_t *cn, listener_t *ls)
{
    (void) conf, ls;
    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    printf("Wait data to %s:%s\n",
        get_addr(str_ip, &cn->sockaddr),
        get_port(str_port, &cn->sockaddr));

    return OK;
}


void handle_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    listener_t *ls;
    connect_t *cn;
    listnode_t *i;
    listnode_t *j;

    err_t err;

    for (i = list_first(conf->listeners); i; i = list_next(i)) {
        /* Search listeners */
        ls = (listener_t *)list_data(i);
        if (FD_ISSET(ls->fd, rfds)) {
            err = handle_new_connection(conf, cn, ls);
            if (err != OK) {
                fprintf(stderr, "handle_new_connection() failed\n");
                fprintf(stderr, "%s\n", get_strerror(err));
                abort();
            }
        }
        /* Search from current listener connections */
        j = list_first(((listener_t *)list_data(i))->connects);
        for ( ; j; j = list_next(j)) {
            /* New data available to read from client */
            cn = (connect_t *)list_data(j);
            if (FD_ISSET(cn->fd, rfds)) {
                err = handle_input_data(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "handle_input_data() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }


            }
            /* Client buffer available to write data */
            if (FD_ISSET(cn->fd, wfds)) {
                err = handle_output_data(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "handle_output_data() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
        }
    }
}



void single_process_cycle(config_t *conf)
{
    struct timeval timeout;
    fd_set rfds;
    fd_set wfds;
    int flag;
    socket_t fdmax;

    while (1) {

        fdmax = select_events(conf, &rfds, &wfds);
        timeout.tv_sec = 10; // TODO: get value from config
        timeout.tv_usec = 0;

        flag = select(fdmax + 1, &rfds, &wfds, NULL, &timeout);

        if (flag == -1) {
            if (errno != EINTR) {
                fprintf(stderr, "select() failed\n");
                abort();
            }
            else {
                printf("interrupted by signal\n");
            }
        }
        else if (!flag) {
            printf("timeout\n");
        }

        else {
            handle_events(conf, &rfds, &wfds);
        }

    } /* while (1) */
}
