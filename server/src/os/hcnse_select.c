#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_HAVE_SELECT)

static fd_set rfds;
static fd_set wfds;


static hcnse_socket_t
hcnse_set_events(hcnse_conf_t *conf)
{
    hcnse_listener_t *listener;
    hcnse_connect_t *connect;
    hcnse_lnode_t *iter1;
    hcnse_lnode_t *iter2;
    hcnse_socket_t fd;
    hcnse_socket_t fdmax = 0;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    for (iter1 = hcnse_list_first(conf->listeners);
                                    iter1; iter1 = hcnse_list_next(iter1)) {
        /* Add listening sockets to read array */
        listener = hcnse_list_cast_ptr(hcnse_listener_t, iter1);
        fd = listener->fd;
        FD_SET(fd, &rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        for (iter2 = hcnse_list_first(listener->connects);
                                    iter2; iter2 = hcnse_list_next(iter2)) {
            /* Add clients sockets to read array */
            connect = hcnse_list_cast_ptr(hcnse_connect_t, iter2);
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

static hcnse_err_t
hcnse_select_add_connect(hcnse_conf_t *conf, hcnse_listener_t *listener)
{
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

    return HCNSE_OK;

failed:
    return err;
}

static hcnse_err_t
hcnse_process_events(hcnse_conf_t *conf)
{
    hcnse_listener_t *listener;
    hcnse_connect_t *connect;
    hcnse_lnode_t *iter1;
    hcnse_lnode_t *iter2;
    hcnse_err_t err;

    for (iter1 = hcnse_list_first(conf->listeners);
                                    iter1; iter1 = hcnse_list_next(iter1)) {
        /* Search listeners */
        listener = hcnse_list_cast_ptr(hcnse_listener_t, iter1);
        if (FD_ISSET(listener->fd, &rfds)) {
            err = hcnse_select_add_connect(conf, listener);
            if (err != HCNSE_OK) {
                goto failed;
            }
            err = event_connect(conf, listener);
            if (err != HCNSE_OK) {
                goto failed;
            }
        }
        /* Search new input connections */
        for (iter2 = hcnse_list_first(listener->connects);
                                    iter2; iter2 = hcnse_list_next(iter2)) {
            connect = hcnse_list_cast_ptr(hcnse_connect_t, iter2);
            if (FD_ISSET(connect->fd, &rfds)) {
                err = event_read(conf, connect);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
            if (FD_ISSET(connect->fd, &wfds)) {
                err = event_write(conf, connect);
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

void
hcnse_select_process_events(hcnse_conf_t *conf)
{
    hcnse_msec_t timer = 10000;
    struct timeval tv;
    struct timeval *timeout;
    int flag;
    hcnse_socket_t fdmax;
    hcnse_err_t err;

    while (1) {

        fdmax = hcnse_set_events(conf);

        tv.tv_sec = 10;
        tv.tv_usec = 0;
        timeout = &tv;

        if (timer == 0) {
            timeout = NULL;
        }
        else {
            tv.tv_sec = (long) (timer / 1000);
            tv.tv_usec = (long) ((timer % 1000) * 1000);
            timeout = &tv;
        }

        flag = select(fdmax + 1, &rfds, &wfds, NULL, timeout);
        if (flag == -1) {
            if (hcnse_get_errno() != EINTR) {
                hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                                    hcnse_get_errno(), "select() failed");
                abort();
            }
            else {
                hcnse_log_error(HCNSE_LOG_INFO, conf->log, HCNSE_OK,
                                            "interrupted by unknown signal");
            }
        }
        else if (!flag) {
            event_timer(conf);
        }
        else {
            err = hcnse_process_events(conf);
            if (err != HCNSE_OK) {
                hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                                        err, "hcnse_process_events() failed");
                abort();
            }
        }
    } /* while (1) */
}
#endif /* HCNSE_HAVE_SELECT */
