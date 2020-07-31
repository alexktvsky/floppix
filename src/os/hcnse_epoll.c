#include "hcnse_portable.h"
#include "hcnse_common.h"

#if (HCNSE_HAVE_EPOLL && HCNSE_LINUX)

static hcnse_err_t hcnse_epoll_add_listener(hcnse_listener_t *listener,
    int flags);
static hcnse_err_t hcnse_epoll_del_listener(hcnse_listener_t *listener,
    int flags);
static hcnse_err_t hcnse_epoll_add_connect(hcnse_conf_t *conf,
    hcnse_listener_t *listener);
static hcnse_err_t hcnse_epoll_del_connect(hcnse_conf_t *conf,
    hcnse_connect_t *connect);
static hcnse_err_t hcnse_epoll_process_events(hcnse_conf_t *conf);
static hcnse_err_t hcnse_epoll_init(hcnse_conf_t *conf);

static struct epoll_event *event_list;
static int max_events;
static int epfd = HCNSE_INVALID_SOCKET;

hcnse_event_actions_t hcnse_event_actions_epoll = {
    hcnse_epoll_add_listener,
    hcnse_epoll_del_listener,
    hcnse_epoll_add_connect,
    hcnse_epoll_del_connect,
    hcnse_epoll_process_events,
    hcnse_epoll_init
};


static hcnse_err_t
hcnse_epoll_add_listener(hcnse_listener_t *listener, int flags)
{
    struct epoll_event ee;
    int op = EPOLL_CTL_ADD; /* When we need to use EPOLL_CTL_MOD? */

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
    int op = EPOLL_CTL_DEL; /* When we need to use EPOLL_CTL_MOD? */

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

    connect = hcnse_try_use_already_exist_node(sizeof(hcnse_connect_t),
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
hcnse_epoll_process_events(hcnse_conf_t *conf)
{
    hcnse_msec_t timer = conf->timer;
    hcnse_listener_t *listener;
    hcnse_connect_t *connect;
    int events;
    int ready_events;
    hcnse_err_t err;


    /* TODO: Change to epoll_pwait when signal handler will be ready */
    events = epoll_wait(epfd, event_list, max_events, (int) timer);
    if (events == -1) {
        if (hcnse_get_errno() != EINTR) {
            return hcnse_get_errno();
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
        hcnse_event_timer(conf);
        return HCNSE_OK;
    }

    for (int i = 0; i < events; i++) {
        if (hcnse_is_listener(event_list[i].data.ptr)) {
            listener = (hcnse_listener_t *) event_list[i].data.ptr;

            err = hcnse_epoll_add_connect(conf, listener);
            if (err != HCNSE_OK) {
                goto failed;
            }

            err = hcnse_event_connect(conf, listener);
            if (err != HCNSE_OK) {
                goto failed;
            }
        }
        else {
            connect = (hcnse_connect_t *) event_list[i].data.ptr;
            ready_events = event_list[i].events;

            if (ready_events & EPOLLIN) {
                err = hcnse_event_read(conf, connect);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
            if ((ready_events & EPOLLOUT) && (connect->ready_to_write)) {
                err = hcnse_event_write(conf, connect);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
        }
    }
    return HCNSE_OK;

failed:
    return err;
}

static hcnse_err_t
hcnse_epoll_init(hcnse_conf_t *conf)
{
    hcnse_listener_t *listener;
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

    for (listener = hcnse_list_first(conf->listeners);
                            listener; listener = hcnse_list_next(listener)) {
        err = hcnse_epoll_add_listener(listener, EPOLLIN|EPOLLET);
        if (err != HCNSE_OK) {
            return err;
        }
    }

    return HCNSE_OK;
}



#endif /* HCNSE_HAVE_EPOLL && HCNSE_LINUX */
