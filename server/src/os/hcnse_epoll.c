#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_HAVE_EPOLL && HCNSE_LINUX)

static struct epoll_event *event_list;
static int max_events;
static int epfd = HCNSE_INVALID_SOCKET;


static hcnse_err_t
hcnse_epoll_add_listener(hcnse_listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_ADD; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        return hcnse_get_errno();
    }

    return HCNSE_OK;
}

static hcnse_err_t
hcnse_epoll_del_listener(hcnse_listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_DEL; // When we need to use EPOLL_CTL_MOD?

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        return hcnse_get_errno();
    }

    return HCNSE_OK;
}

static hcnse_err_t
hcnse_epoll_add_connect(hcnse_conf_t *conf, hcnse_listener_t *listener)
{
    struct epoll_event ee;
    hcnse_connect_t *connect;
    hcnse_err_t err;

    connect = hcnse_try_use_already_exist_node(hcnse_connect_t,
                            conf->free_connects, listener->connects);
    if (!connect) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_connection_accept(connect, listener);
    if (err != HCNSE_OK) {
        goto failed;
    }

    ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
    ee.data.ptr = connect;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, connect->fd, &ee) == -1) {
        return hcnse_get_errno();
    }

    return HCNSE_OK;

failed:
    return err;
}

static hcnse_err_t
hcnse_epoll_del_connect(hcnse_conf_t *conf, hcnse_connect_t *connect)
{
    struct epoll_event ee;

    ee.events = 0;
    ee.data.ptr = NULL;

    if (epoll_ctl(epfd, EPOLL_CTL_DEL, connect->fd, &ee) == -1) {
        return hcnse_get_errno();
    }

    hcnse_connection_cleanup(connect);
    hcnse_list_reserve_node(connect, conf->free_connects,
                                        connect->owner->connects);

    return HCNSE_OK;
}

static hcnse_err_t
hcnse_epoll_init(hcnse_conf_t *conf)
{
    hcnse_err_t err;

    max_events = hcnse_list_size(conf->listeners);

    epfd = epoll_create1(0);
    if (epfd == -1) {
        return hcnse_get_errno();
    }

    event_list = hcnse_malloc(sizeof(struct epoll_event) * max_events);
    if (!event_list) {
        return hcnse_get_errno();
    }

    hcnse_lnode_t *iter1;
    hcnse_listener_t *listener;
    for (iter1 = hcnse_list_first(conf->listeners);
                                        iter1; iter1 = hcnse_list_next(iter1)) {
        listener = hcnse_list_cast_ptr(hcnse_listener_t, iter1);
        err = hcnse_epoll_add_listener(listener, EPOLLIN|EPOLLET);
        if (err != HCNSE_OK) {
            return err;
        }
    }
    return HCNSE_OK;
}

static hcnse_err_t
hcnse_process_events(hcnse_conf_t *conf, int n)
{
    int flags;
    hcnse_err_t err;

    for (int i = 0; i < n; i++) {
        /* Search new input connections */
        if (hcnse_is_listener(event_list[i].data.ptr)) {
            err = hcnse_epoll_add_connect(conf, event_list[i].data.ptr);
            if (err != HCNSE_OK) {
                goto failed;
            }
            err = event_connect(conf, event_list[i].data.ptr);
            if (err != HCNSE_OK) {
                goto failed;
            }
        }
        else {
            flags = event_list[i].events;
            if (flags & EPOLLIN) {
                err = event_read(conf, event_list[i].data.ptr);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
            if (flags & EPOLLOUT) {
                if (((hcnse_connect_t *) event_list[i].data.ptr)->want_to_write) {
                    err = event_write(conf, event_list[i].data.ptr);
                    if (err != HCNSE_OK) {
                        goto failed;
                    }
                }
            }
        }
    }

    return HCNSE_OK;

failed:
    return err;
}

void
hcnse_epoll_process_events(hcnse_conf_t *conf)
{
    int n;
    hcnse_msec_t timer = 10000;
    hcnse_err_t err;

    err = hcnse_epoll_init(conf);
    if (err != HCNSE_OK) {
        hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                                    err, "epoll_init() failed");
        abort();
    }

    while (1) {
        /* TODO: Change to epoll_pwait when signal handler will be ready */
        n = epoll_wait(epfd, event_list, max_events, (int) timer);
        if (n == -1) {
            if (hcnse_get_errno() != EINTR) {
                hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                                    hcnse_get_errno(), "epoll_wait() failed");
                abort();
            }
            // else if (sighup_caught) {
            //     printf("interrupted by SIGHUP\n");
            //     sighup_caught = 0;
            // }
            else {
                hcnse_log_debug(HCNSE_LOG_DEBUG, conf->log,
                                            "interrupted by unknown signal");
            }
        }
        else if (!n) {
            event_timer(conf);
        }
        else {
            err = hcnse_process_events(conf, n);
            if (err != HCNSE_OK) {
                hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                        err, "hcnse_process_events() failed");
                abort();
            }
        }
    } /* while (1) */
}

#endif /* HCNSE_HAVE_EPOLL && HCNSE_LINUX */
