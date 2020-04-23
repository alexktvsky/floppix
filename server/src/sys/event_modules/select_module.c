#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"

#if (USE_SELECT_MODULE)
#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#endif

#include "errors.h"
#include "sys_errno.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "log.h"
#include "config.h"
#include "events.h"

static fd_set rfds;
static fd_set wfds;


static socket_t set_events(config_t *conf)
{
    listener_t *listener;
    connect_t *connect;
    listnode_t *iter1;
    listnode_t *iter2;
    socket_t fd;
    socket_t fdmax = 0;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    for (iter1 = list_first(conf->listeners); iter1; iter1 = list_next(iter1)) {
        /* Add listening sockets to read array */
        listener = list_cast_ptr(listener_t, iter1);
        fd = listener->fd;
        FD_SET(fd, &rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        for (iter2 = list_first(listener->connects); iter2; iter2 = list_next(iter2)) {
            /* Add clients sockets to read array */
            connect = list_cast_ptr(connect_t, iter2);
            fd = connect->fd;
            FD_SET(fd, &rfds);
            if (fdmax < fd) {
                fdmax = fd;
            }

            /* Add clients sockets to write array */
            if (!connect->want_to_write) {
                continue;
            }
            fd = connect->fd;
            FD_SET(fd, &wfds);
            if (fdmax < fd) {
                fdmax = fd;
            }
        }
    }
    return fdmax;
}

static err_t select_add_connect(config_t *conf, listener_t *listener)
{
    connect_t *connect;
    err_t err;
    connect = try_use_already_exist_node(connect_t,
                            conf->free_connects, listener->connects);
    if (!connect) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    err = connection_accept(connect, listener);
    if (err != OK) {
        goto failed;
    }

    return OK;

failed:
    return err;
}

static err_t process_events(config_t *conf)
{
    listener_t *listener;
    connect_t *connect;
    listnode_t *iter1;
    listnode_t *iter2;
    err_t err;

    for (iter1 = list_first(conf->listeners); iter1; iter1 = list_next(iter1)) {
        /* Search listeners */
        listener = list_cast_ptr(listener_t, iter1);
        if (FD_ISSET(listener->fd, &rfds)) {
            err = select_add_connect(conf, listener);
            if (err != OK) {
                goto failed;
            }
            err = event_connect(conf, listener);
            if (err != OK) {
                goto failed;
            }
        }
        /* Search new input connections */
        for (iter2 = list_first(listener->connects); iter2; iter2 = list_next(iter2)) {
            connect = list_cast_ptr(connect_t, iter2);
            if (FD_ISSET(connect->fd, &rfds)) {
                err = event_read(conf, connect);
                if (err != OK) {
                    goto failed;
                }
            }
            if (FD_ISSET(connect->fd, &wfds)) {
                err = event_write(conf, connect);
                if (err != OK) {
                    goto failed;
                }
            }
        }
    }

    return OK;

failed:
    return err;
}

void select_process_events(config_t *conf)
{
    struct timeval tv;
    struct timeval *timeout;
    int flag;
    socket_t fdmax;
    err_t err;

    while (1) {

        fdmax = set_events(conf);

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
            if (sys_get_errno() != EINTR) {
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
            err = process_events(conf);
            if (err != OK) {
                fprintf(stderr, "%s\n", "process_events() failed");
                fprintf(stderr, "%s\n", err_strerror(err));
                fprintf(stderr, "%s\n", sys_strerror(sys_get_errno()));
                abort();
            }
        }
    } /* while (1) */
}
#endif /* USE_SELECT_MODULE */
