#include "fpx_portable.h"
#include "fpx_core.h"

#if (FPX_HAVE_IOCP && FPX_WIN32)

#define FPX_IOCP_ACCEPT 0
#define FPX_IOCP_RW     1

#define FPX_NTHREAD     1

typedef struct {
    WSAOVERLAPPED ovlp;
    void *instance;
} fpx_ovlp_t;

static fpx_err_t fpx_iocp_add_listener(fpx_listener_t *listener);
static fpx_err_t fpx_iocp_process_events(fpx_conf_t *conf);
static fpx_err_t fpx_iocp_init(fpx_conf_t *conf);

fpx_event_actions_t fpx_event_actions_iocp = {
    NULL,
    NULL,
    NULL,
    NULL,
    fpx_iocp_process_events,
    fpx_iocp_init,
};

static HANDLE iocp;

static fpx_err_t
fpx_iocp_add_connect(fpx_listener_t *listener, fpx_connect_t *connect)
{
    int key = FPX_IOCP_RW;

    /* "(HANDLE) listener->fd" is good idea? */
    if (CreateIoCompletionPort((HANDLE) listener->fd, iocp, key, 0) == NULL) {
        return fpx_get_errno();
    }

    return FPX_OK;
}

static fpx_err_t
fpx_iocp_add_listener(fpx_listener_t *listener)
{
    int key = FPX_IOCP_ACCEPT;
    /* "(HANDLE) listener->fd" is good idea? */
    if (CreateIoCompletionPort((HANDLE) listener->fd, iocp, key, 0) == NULL) {
        return fpx_get_errno();
    }

    return FPX_OK;
}

static fpx_err_t
fpx_iocp_process_events(fpx_conf_t *conf)
{
    int events;
    uint32_t bytes;
    fpx_msec_t timer = conf->timer;
    uint32_t key;
    LPOVERLAPPED *ovlp;
    fpx_err_t err;

    events = GetQueuedCompletionStatus(iocp, (LPDWORD) &bytes,
        (PULONG_PTR) &key, (LPOVERLAPPED *) &ovlp, timer);
    if (events == 0) {
        err = fpx_get_errno();
    }
    else {
        err = 0;
    }

    if (err) {
        if (ovlp == NULL) {
            if (err != WAIT_TIMEOUT) {
                goto failed;
            }
        }
        else if (err == ERROR_NETNAME_DELETED || /* the socket was closed */
            err == ERROR_OPERATION_ABORTED)
        { /* the operation was canceled */
            return FPX_OK;
        }
        else {
            /* GetQueuedCompletionStatus() returned operation error */
        }
    }
    else {
        if (ovlp == NULL) {
            /* timeout */
        }
    }

    switch (key) {
    case FPX_IOCP_ACCEPT:
        printf("FPX_IOCP_ACCEPT\n");
        break;
    case FPX_IOCP_RW:
        printf("FPX_IOCP_RW\n");
        break;
    }

    return FPX_OK;

failed:
    return err;
}

static fpx_err_t
fpx_iocp_init(fpx_conf_t *conf)
{
    fpx_listener_t *listener;
    fpx_err_t err;

    iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, FPX_NTHREAD);
    if (iocp == NULL) {
        err = fpx_get_errno();
        goto failed;
    }

    for (listener = fpx_list_first(conf->listeners); listener;
         listener = fpx_list_next(listener))
    {
        err = fpx_iocp_add_listener(listener);
        if (err != FPX_OK) {
            goto failed;
        }
    }

    return FPX_OK;

failed:
    return err;
}

#endif
