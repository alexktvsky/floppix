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
#include "sys_files.h"
#include "log.h"
#include "config.h"
#include "cycle.h"
#include "events.h"


socket_t select_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    socket_t fd;
    socket_t fdmax = 0;

    FD_ZERO(rfds);
    FD_ZERO(wfds);

    for (listener_t *ls = (listener_t *) list_first(conf->listeners);
                                    ls; ls = (listener_t *) list_next(ls)) {
        /* Add listening sockets to read array */
        fd = ls->fd;
        FD_SET(fd, rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        for (connect_t *cn = (connect_t *) list_first(ls->connects);
                                    cn; cn = (connect_t *) list_next(cn)) {
            /* Add clients sockets to read array */
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
    err_t err;

    for (listener_t *ls = (listener_t *) list_first(conf->listeners);
                                    ls; ls = (listener_t *) list_next(ls)) {
        /* Search listeners */
        if (FD_ISSET(ls->fd, rfds)) {
            err = event_connect(conf, ls);
            if (err != OK) {
                fprintf(stderr, "event_connect() failed\n");
                fprintf(stderr, "%s\n", err_strerror(err));
                abort();
            }
        }
        /* Search from current listener connections */
        for (connect_t *cn = (connect_t *) list_first(ls->connects);
                                    cn; cn = (connect_t *) list_next(cn)) {
            if (FD_ISSET(cn->fd, rfds)) {
                err = event_read(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "event_read() failed\n");
                    fprintf(stderr, "%s\n", err_strerror(err));
                    abort();
                }
            }
            if (FD_ISSET(cn->fd, wfds)) {
                err = event_write(conf, cn, ls);
                if (err != OK) {
                    fprintf(stderr, "event_write() failed\n");
                    fprintf(stderr, "%s\n", err_strerror(err));
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
