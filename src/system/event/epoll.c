#include "fpx_portable.h"
#include "fpx_core.h"

#if (FPX_HAVE_EPOLL && FPX_LINUX)

static fpx_err_t fpx_epoll_add_listener(fpx_listener_t *listener, int flags);
static fpx_err_t fpx_epoll_del_listener(fpx_listener_t *listener, int flags);
static fpx_err_t fpx_epoll_add_connect(fpx_conf_t *conf,
    fpx_listener_t *listener);
static fpx_err_t fpx_epoll_del_connect(fpx_conf_t *conf,
    fpx_connect_t *connect);
static fpx_err_t fpx_epoll_process_events(fpx_conf_t *conf);
static fpx_err_t fpx_epoll_init(fpx_conf_t *conf);

static struct epoll_event *event_list;
static int max_events;
static int epfd = FPX_INVALID_SOCKET;

fpx_event_actions_t fpx_event_actions_epoll = {
    fpx_epoll_add_listener,
    fpx_epoll_del_listener,
    fpx_epoll_add_connect,
    fpx_epoll_del_connect,
    fpx_epoll_process_events,
    fpx_epoll_init,
};

static fpx_err_t
fpx_epoll_add_listener(fpx_listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_ADD; /* When we need to use EPOLL_CTL_MOD? */

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        return fpx_get_errno();
    }

    return FPX_OK;
}

static fpx_err_t
fpx_epoll_del_listener(fpx_listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_DEL; /* When we need to use EPOLL_CTL_MOD? */

    ee.events = flags;
    ee.data.ptr = listener;

    if (epoll_ctl(epfd, op, listener->fd, &ee) == -1) {
        return fpx_get_errno();
    }

    return FPX_OK;
}

static fpx_err_t
fpx_epoll_add_connect(fpx_conf_t *conf, fpx_listener_t *listener)
{
    struct epoll_event ee;
    fpx_connect_t *connect;
    fpx_err_t err;

    connect = fpx_try_use_already_exist_node(sizeof(fpx_connect_t),
        conf->free_connects, listener->connects);
    if (!connect) {
        err = fpx_get_errno();
        goto failed;
    }

    err = fpx_connection_accept(connect, listener);
    if (err != FPX_OK) {
        goto failed;
    }

    ee.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
    ee.data.ptr = connect;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, connect->fd, &ee) == -1) {
        return fpx_get_errno();
    }

    return FPX_OK;

failed:
    return err;
}

static fpx_err_t
fpx_epoll_del_connect(fpx_conf_t *conf, fpx_connect_t *connect)
{
    struct epoll_event ee;

    ee.events = 0;
    ee.data.ptr = NULL;

    if (epoll_ctl(epfd, EPOLL_CTL_DEL, connect->fd, &ee) == -1) {
        return fpx_get_errno();
    }

    fpx_connection_cleanup(connect);
    fpx_list_reserve_node(connect, conf->free_connects,
        connect->owner->connects);

    return FPX_OK;
}

static fpx_err_t
fpx_epoll_process_events(fpx_conf_t *conf)
{
    fpx_msec_t timer = conf->timer;
    fpx_listener_t *listener;
    fpx_connect_t *connect;
    int events;
    int ready_events;
    fpx_err_t err;

    /* TODO: Change to epoll_pwait when signal handler will be ready */
    events = epoll_wait(epfd, event_list, max_events, (int) timer);
    if (events == -1) {
        if (fpx_get_errno() != EINTR) {
            return fpx_get_errno();
        }
#if 0
        else if (sighup_caught) {
            printf("interrupted by SIGHUP\events");
            sighup_caught = 0;
        }
#endif
        else {
            /* Interrupted by unknown signal */
        }
    }
    if (events == 0) {
        fpx_event_timer(conf);
        return FPX_OK;
    }

    for (int i = 0; i < events; ++i) {
        if (fpx_is_listener(event_list[i].data.ptr)) {
            listener = (fpx_listener_t *) event_list[i].data.ptr;

            err = fpx_epoll_add_connect(conf, listener);
            if (err != FPX_OK) {
                goto failed;
            }

            err = fpx_event_connect(conf, listener);
            if (err != FPX_OK) {
                goto failed;
            }
        }
        else {
            connect = (fpx_connect_t *) event_list[i].data.ptr;
            ready_events = event_list[i].events;

            if (ready_events & EPOLLIN) {
                err = fpx_event_read(conf, connect);
                if (err != FPX_OK) {
                    goto failed;
                }
            }
            if ((ready_events & EPOLLOUT) && (connect->ready_to_write)) {
                err = fpx_event_write(conf, connect);
                if (err != FPX_OK) {
                    goto failed;
                }
            }
        }
    }
    return FPX_OK;

failed:
    return err;
}

static fpx_err_t
fpx_epoll_init(fpx_conf_t *conf)
{
    fpx_listener_t *listener;
    fpx_err_t err;

    max_events = fpx_list_size(conf->listeners);

    epfd = epoll_create1(0);
    if (epfd == -1) {
        return fpx_get_errno();
    }

    event_list = fpx_malloc(sizeof(struct epoll_event) * max_events);
    if (!event_list) {
        return fpx_get_errno();
    }

    for (listener = fpx_list_first(conf->listeners); listener;
         listener = fpx_list_next(listener))
    {
        err = fpx_epoll_add_listener(listener, EPOLLIN | EPOLLET);
        if (err != FPX_OK) {
            return err;
        }
    }

    return FPX_OK;
}

#endif /* FPX_HAVE_EPOLL && FPX_LINUX */
