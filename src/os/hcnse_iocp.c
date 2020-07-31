#include "hcnse_portable.h"
#include "hcnse_common.h"

#if (HCNSE_HAVE_IOCP && HCNSE_WIN32)

#define HCNSE_IOCP_ACCEPT      0
#define HCNSE_IOCP_RW          1

#define HCNSE_NTHREAD  1

typedef struct {
    WSAOVERLAPPED ovlp;
    void *instance;
} hcnse_ovlp_t;







static hcnse_err_t hcnse_iocp_add_listener(hcnse_listener_t *listener);
static hcnse_err_t hcnse_iocp_process_events(hcnse_conf_t *conf);
static hcnse_err_t hcnse_iocp_init(hcnse_conf_t *conf);



hcnse_event_actions_t hcnse_event_actions_iocp = {
    NULL,
    NULL,
    NULL,
    NULL,
    hcnse_iocp_process_events,
    hcnse_iocp_init
};






static HANDLE iocp;

static hcnse_err_t
hcnse_iocp_add_connect(hcnse_listener_t *listener, hcnse_connect_t *connect)
{
    int key = HCNSE_IOCP_RW;

    /* "(HANDLE) listener->fd" is good idea? */
    if (CreateIoCompletionPort((HANDLE) listener->fd, iocp, key, 0) == NULL) {
        return hcnse_get_errno();
    }




    return HCNSE_OK;
}


static hcnse_err_t
hcnse_iocp_add_listener(hcnse_listener_t *listener)
{
    int key = HCNSE_IOCP_ACCEPT;
    /* "(HANDLE) listener->fd" is good idea? */
    if (CreateIoCompletionPort((HANDLE) listener->fd, iocp, key, 0) == NULL) {
        return hcnse_get_errno();
    }




    return HCNSE_OK;
}



static hcnse_err_t
hcnse_iocp_process_events(hcnse_conf_t *conf)
{
    int events;
    uint32_t bytes;
    hcnse_msec_t timer = conf->timer;
    uint32_t key;
    LPOVERLAPPED *ovlp;
    hcnse_err_t err;


    events = GetQueuedCompletionStatus(iocp, (LPDWORD) &bytes,
                (PULONG_PTR) &key, (LPOVERLAPPED *) &ovlp, timer);
    if (events == 0) {
        err = hcnse_get_errno();
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
            err == ERROR_OPERATION_ABORTED) { /* the operation was canceled */
            return HCNSE_OK;
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
    case HCNSE_IOCP_ACCEPT:
        printf("HCNSE_IOCP_ACCEPT\n");
        break;
    case HCNSE_IOCP_RW:
        printf("HCNSE_IOCP_RW\n");
        break;
    }

    return HCNSE_OK;

failed:
    return err;
}


static hcnse_err_t
hcnse_iocp_init(hcnse_conf_t *conf)
{
    hcnse_listener_t *listener;
    hcnse_err_t err;


    iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, HCNSE_NTHREAD);
    if (iocp == NULL) {
        err = hcnse_get_errno();
        goto failed;
    }

    for (listener = hcnse_list_first(conf->listeners);
                    listener; listener = hcnse_list_next(listener)) {
        err = hcnse_iocp_add_listener(listener);
        if (err != HCNSE_OK) {
            goto failed;
        }
    }

    return HCNSE_OK;

failed:
    return err;
}

#endif
