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
#include "events.h"


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
        ls = (listener_t *) list_data(i);
        fd = ls->fd;
        FD_SET(fd, rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        j = list_first(ls->connects);
        for ( ; j; j = list_next(j)) {
            /* Add clients sockets to read array */
            cn = (connect_t *) list_data(j);
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


void handle_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    listener_t *ls;
    connect_t *cn;
    listnode_t *i;
    listnode_t *j;

    err_t err;

    for (i = list_first(conf->listeners); i; i = list_next(i)) {
        /* Search listeners */
        ls = (listener_t *) list_data(i);
        if (FD_ISSET(ls->fd, rfds)) {
            err = event_connect(conf, ls);
            if (err != OK) {
                fprintf(stderr, "event_connect() failed\n");
                fprintf(stderr, "%s\n", get_strerror(err));
                abort();
            }
        }
        /* Search from current listener connections */
        j = list_first(ls->connects);
        for ( ; j; j = list_next(j)) {
            cn = (connect_t *) list_data(j);
            if (FD_ISSET(cn->fd, rfds)) {
                err = event_read(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "event_read() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
            if (FD_ISSET(cn->fd, wfds)) {
                err = event_write(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "event_write() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
        }
    }
    return;
}



void single_process_cycle(config_t *conf)
{
    struct timeval tv;
    struct timeval *timeout;
    fd_set rfds;
    fd_set wfds;
    int flag;
    socket_t fdmax;

    while (1) {

        fdmax = select_events(conf, &rfds, &wfds);

        tv.tv_sec = 10;
        tv.tv_usec = 0;
        timeout = &tv;

        // if (timer == 0) {
        //     timeout = NULL;
        // }
        // else {
        //     tv.tv_sec = (long) (timer / 1000);
        //     tv.tv_usec = (long) ((timer % 1000) * 1000);
        //     timeout = &tv;
        // }

        flag = select(fdmax + 1, &rfds, &wfds, NULL, timeout);

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
            event_timer(conf);
        }

        else {
            handle_events(conf, &rfds, &wfds);
        }

    } /* while (1) */
}
