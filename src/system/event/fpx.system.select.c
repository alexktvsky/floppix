#include "fpx_portable.h"
#include "fpx_core.h"

#if (FPX_HAVE_SELECT)

static fd_set rfds;
static fd_set wfds;

static fpx_socket_t
fpx_set_events(fpx_conf_t *conf)
{
    fpx_listener_t *listener;
    fpx_connect_t *connect;
    fpx_socket_t fd;
    fpx_socket_t fdmax = 0;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    for (listener = fpx_list_first(conf->listeners); listener;
         listener = fpx_list_next(listener))
    {
        /* Add listening sockets to read array */
        fd = listener->fd;
        FD_SET(fd, &rfds);
        if (fdmax < fd) {
            fdmax = fd;
        }

        for (connect = fpx_list_first(listener->connects); connect;
             connect = fpx_list_next(connect))
        {
            /* Add clients sockets to read array */
            fd = connect->fd;
            FD_SET(fd, &rfds);
            if (fdmax < fd) {
                fdmax = fd;
            }
            /* Add clients sockets to write array */
            if (!connect->ready_to_write) {
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

static fpx_err_t
fpx_select_add_connect(fpx_conf_t *conf, fpx_listener_t *listener)
{
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

    return FPX_OK;

failed:
    return err;
}

static fpx_err_t
fpx_process_events(fpx_conf_t *conf)
{
    fpx_listener_t *listener;
    fpx_connect_t *connect;
    fpx_err_t err;

    for (listener = fpx_list_first(conf->listeners); listener;
         listener = fpx_list_next(listener))
    {
        /* Search listeners */
        if (FD_ISSET(listener->fd, &rfds)) {
            err = fpx_select_add_connect(conf, listener);
            if (err != FPX_OK) {
                goto failed;
            }
            err = fpx_event_connect(conf, listener);
            if (err != FPX_OK) {
                goto failed;
            }
        }
        /* Search new input connections */
        for (connect = fpx_list_first(listener->connects); connect;
             connect = fpx_list_next(connect))
        {
            if (FD_ISSET(connect->fd, &rfds)) {
                err = fpx_event_read(conf, connect);
                if (err != FPX_OK) {
                    goto failed;
                }
            }
            if (FD_ISSET(connect->fd, &wfds)) {
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

void
fpx_select_process_events(fpx_conf_t *conf)
{
    fpx_msec_t timer = conf->timer;
    struct timeval tv;
    struct timeval *timeout;
    int flag;
    fpx_socket_t fdmax;
    fpx_err_t err;

    while (1) {

        fdmax = fpx_set_events(conf);

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
            if (fpx_get_errno() != EINTR) {
                fpx_log_error(FPX_LOG_EMERG, conf->log, fpx_get_errno(),
                    "select() failed");
                abort();
            }
            else {
                fpx_log_error(FPX_LOG_INFO, conf->log, FPX_OK,
                    "interrupted by unknown signal");
            }
        }
        else if (!flag) {
            fpx_event_timer(conf);
        }
        else {
            err = fpx_process_events(conf);
            if (err != FPX_OK) {
                fpx_log_error(FPX_LOG_EMERG, conf->log, err,
                    "fpx_process_events() failed");
                abort();
            }
        }
    } /* while (1) */
}
#endif /* FPX_HAVE_SELECT */
