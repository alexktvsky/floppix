#include "syshead.h"

#ifndef USE_SELECT_MODULE
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
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "config.h"
#include "log.h"
#include "events.h"


static struct epoll_event *events;
static int max_events;
static int epfd = -1;


/* XXX: Work for new connections too */
static err_t epoll_add_event(socket_t fd, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_ADD; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.fd = fd;

    if (epoll_ctl(epfd, op, fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;
}

static err_t epoll_del_event(socket_t fd, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_DEL; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.fd = fd;

    if (epoll_ctl(epfd, op, fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;
}

static err_t epoll_add_connect(socket_t fd)
{
    struct epoll_event ee;
    ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

    return OK;
}

static err_t epoll_del_connect(socket_t fd)
{
    struct epoll_event ee;
    ee.events = 0;
    ee.data.fd = -1;

    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ee) == -1) {
        fprintf(stderr, "epoll_ctl() failed\n");
        return ERR_FAILED;
    }

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

    events = malloc(sizeof(struct epoll_event) * max_events);
    if (!events) {
        return ERR_MEM_ALLOC;
    }

    listnode_t *iter1;
    listener_t *listener;
    for (iter1 = list_first(conf->listeners); iter1; iter1 = list_next(iter1)) {
        listener = list_cast_ptr(listener_t, iter1);
        err = epoll_add_event(listener->fd, EPOLLIN|EPOLLOUT);
        if (err != OK) {
            return err;
        }
    }
    return OK;
}

static err_t process_events(config_t *conf, int n)
{
    listener_t *listener;
    connect_t *connect;
    listnode_t *iter1;
    listnode_t *iter2;
    err_t err;

    for (int i = 0; i < n; i++) {
        for (iter1 = list_first(conf->listeners); iter1; iter1 = list_next(iter1)) {
            listener = list_cast_ptr(listener_t, iter1);
            if (listener->fd == events[i].data.fd) {
                err = event_connect(conf, list_cast_ptr(listener_t, iter1));
                if (err != OK) {
                    goto failed;
                }
                /* Get new connect from end of list */
                iter2 = list_last(listener->connects);
                connect = list_cast_ptr(connect_t, iter2);
                err = epoll_add_connect(connect->fd);
                if (err != OK) {
                    fprintf(stderr, "%s\n", "epoll_add_connect() failed");
                    goto failed;
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
    int timeout = 5000;
    err_t err;

    if (epoll_init(conf) != OK) {
        fprintf(stderr, "%s\n", "epoll_init() failed");
        abort();
    }

    while (1) {
        /* TODO: Change to epoll_pwait when signal handler will be ready */
        n = epoll_wait(epfd, events, max_events, timeout);
        if (n == -1) {
            if (sys_errno != EINTR) {
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
                fprintf(stderr, "%s\n", sys_strerror(sys_errno));
                abort();
            }
        }
    } /* while (1) */
}
#endif /* SYSTEM_LINUX */
#endif /* USE_SELECT_MODULE */
