#include "syshead.h"

#if !(EVENTS_USE_SELECT)
#if (SYSTEM_LINUX)
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>

#include "errors.h"
#include "sys_errno.h"
#include "sys_memory.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "config.h"
#include "log.h"
#include "events.h"


static struct epoll_event *event_list;
static int max_events;
static int epfd = SYS_INVALID_SOCKET;


static err_t epoll_add_listener(listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_ADD; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;
}

static err_t epoll_del_listener(listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_DEL; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;
}

static err_t epoll_add_connect(config_t *conf, listener_t *listener)
{
    struct epoll_event ee;
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

    ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
    ee.data.ptr = connect;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, connect->fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;

failed:

    return err;
}

static err_t epoll_del_connect(config_t *conf, connect_t *connect)
{
    struct epoll_event ee;

    ee.events = 0;
    ee.data.ptr = NULL;

    if (epoll_ctl(epfd, EPOLL_CTL_DEL, connect->fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    connection_cleanup(connect);
    reserve_node(connect, conf->free_connects, connect->owner->connects);

    return OK;
}

static err_t epoll_init(config_t *conf)
{
    err_t err;

    max_events = list_size(conf->listeners);

    epfd = epoll_create1(0);
    if (epfd == -1) {
        return ERR_FAILED;
    }

    event_list = sys_alloc(sizeof(struct epoll_event) * max_events);
    if (!event_list) {
        return ERR_MEM_ALLOC;
    }

    listnode_t *iter1;
    listener_t *listener;
    for (iter1 = list_first(conf->listeners); iter1; iter1 = list_next(iter1)) {
        listener = list_cast_ptr(listener_t, iter1);
        err = epoll_add_listener(listener, EPOLLIN|EPOLLET);
        if (err != OK) {
            return err;
        }
    }
    return OK;
}

static err_t process_events(config_t *conf, int n)
{
    int flags;
    err_t err;

    for (int i = 0; i < n; i++) {
        /* Search new input connections */
        if (connection_identifier(event_list[i].data.ptr) == IS_LISTENER) {
            err = epoll_add_connect(conf, event_list[i].data.ptr);
            if (err != OK) {
                goto failed;
            }
            err = event_connect(conf, event_list[i].data.ptr);
            if (err != OK) {
                goto failed;
            }
        }
        else {
            flags = event_list[i].events;
            if (flags & EPOLLIN) {
                err = event_read(conf, event_list[i].data.ptr);
                if (err != OK) {
                    goto failed;
                }
            }
            if (flags & EPOLLOUT) {
                if (((connect_t *) event_list[i].data.ptr)->want_to_write) {
                    err = event_write(conf, event_list[i].data.ptr);
                    if (err != OK) {
                        goto failed;
                    }
                }
            }
        }
    }


    return OK;

failed:
    return err;
}

void epoll_process_events(config_t *conf)
{
    int n;
    int timeout = 10000;
    err_t err;

    if (epoll_init(conf) != OK) {
        fprintf(stderr, "%s\n", "epoll_init() failed");
        abort();
    }

    while (1) {
        /* TODO: Change to epoll_pwait when signal handler will be ready */
        n = epoll_wait(epfd, event_list, max_events, timeout);
        if (n == -1) {
            if (sys_get_errno() != EINTR) {
                fprintf(stderr, "epoll_wait() failed\n");
                abort();
            }
            // else if (sighup_caught) {
            //     printf("interrupted by SIGHUP\n");
            //     sighup_caught = 0;
            // }
            else {
                printf("interrupted by unknown signal\n");
            }
        }
        else if (!n) {
            event_timer(conf);
        }
        else {
            err = process_events(conf, n);
            if (err != OK) {
                fprintf(stderr, "%s\n", "process_events() failed");
                fprintf(stderr, "%s\n", err_strerror(err));
                fprintf(stderr, "%s\n", sys_strerror(sys_get_errno()));
                abort();
            }
        }
    } /* while (1) */
}
#endif /* SYSTEM_LINUX */
#endif /* EVENTS_USE_SELECT */
