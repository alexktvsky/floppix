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


static socket_t set_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    socket_t fd;
    socket_t fdmax = 0;

    FD_ZERO(rfds);
    FD_ZERO(wfds);

    list_foreach(listener_t *, listener, conf->listeners) {
        /* Add listening sockets to read array */
        fd = listener->fd;
        FD_SET(fd, rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        list_foreach(connect_t *, connect, listener->connects) {
            /* Add clients sockets to read array */
            fd = connect->fd;
            FD_SET(fd, rfds);
            if (fdmax < fd) {
                fdmax = fd;
            }

            /* Add clients sockets to write array */
            if (!connect->want_to_write) {
                continue;
            }
            fd = connect->fd;
            FD_SET(fd, wfds);
            if (fdmax < fd) {
                fdmax = fd;
            }
        }
    }
    return fdmax;
}


static void handle_events(config_t *conf, fd_set *rfds, fd_set *wfds)
{
    err_t err;

    list_foreach(listener_t *, listener, conf->listeners) {
        /* Search listeners */
        if (FD_ISSET(listener->fd, rfds)) {
            err = event_connect(conf, listener);
            if (err != OK) {
                fprintf(stderr, "event_connect() failed\n");
                fprintf(stderr, "%s\n", err_strerror(err));
                abort();
            }
        }
        /* Search from current listener connections */
        list_foreach(connect_t *, connect, listener->connects) {
            if (FD_ISSET(connect->fd, rfds)) {
                err = event_read(conf, connect, listener);
                if (err != OK) {
                    fprintf(stderr, "event_read() failed\n");
                    fprintf(stderr, "%s\n", err_strerror(err));
                    abort();
                }
            }
            if (FD_ISSET(connect->fd, wfds)) {
                err = event_write(conf, connect, listener);
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


void select_process_events(config_t *conf)
{
    struct timeval tv;
    struct timeval *timeout;
    fd_set rfds;
    fd_set wfds;
    int flag;
    socket_t fdmax;

    while (1) {

        fdmax = set_events(conf, &rfds, &wfds);

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
