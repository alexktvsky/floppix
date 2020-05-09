#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_HAVE_IOCP && HCNSE_WIN32)

static HANDLE iocp;


int n_threads = 1; // move to config


static hcnse_err_t
hcnse_iocp_add_listener(hcnse_conf_t *conf,
    hcnse_listener_t *listener, uint32_t key)
{
    /* ((HANDLE) listener->fd) is good idea? */
    if (CreateIoCompletionPort((HANDLE) listener->fd, iocp, key, 0) == NULL) {
        return hcnse_get_errno();
    }




    return HCNSE_OK;
}


void
hcnse_iocp_process_events(hcnse_conf_t *conf)
{
    int flag;
    uint64_t bytes;
    hcnse_msec_t timer = conf->timer;
    uint32_t key;
    LPOVERLAPPED *ovlp;
    hcnse_err_t err;

    iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, n_threads);
    if (iocp == NULL) {
        hcnse_log_error(HCNSE_LOG_EMERG, conf->log,
                        hcnse_get_errno(), "CreateIoCompletionPort() failed");
        goto failed;
    }



    while (1) {
        flag = GetQueuedCompletionStatus(iocp, (LPDWORD) &bytes,
                            (PULONG_PTR) &key, (LPOVERLAPPED *) &ovlp, timer);
        if (flag == 0) {
            err = errno;
        }
        else {
            err = 0;
        }

        if (err) {
            if (ovlp == NULL) {
                if (err != WAIT_TIMEOUT) {
                    fprintf(stderr, "%s\n", "GetQueuedCompletionStatus() failed");
                    return 1;
                }
            }
            else if (err == ERROR_NETNAME_DELETED || /* the socket was closed */
                        err == ERROR_OPERATION_ABORTED) { /* the operation was canceled */
                printf("iocp: aborted event\n");
            }
            else {
                printf("GetQueuedCompletionStatus() returned operation error\n");
            }
        }
        else {
            if (ovlp == NULL) {
                printf("timeout\n");
            }
        }

        switch (key) {
        case EVENT1_KEY:
            printf("EVENT1\n");
            break;
        }
    } /* while (1) */


failed:
    abort();
}


#endif
