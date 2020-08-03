#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)
hcnse_err_t
hcnse_send_wakeup_signal(hcnse_thread_t *thread)
{
    if (pthread_kill(thread->handle, SIGALRM) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_wait_wakeup_signal(hcnse_msec_t ms)
{
    if (usleep(ms * 1000) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

#elif (HCNSE_WIN32)
hcnse_err_t
hcnse_send_wakeup_signal(hcnse_thread_t *thread)
{
    if (PostThreadMessageA(GetThreadId(thread->handle), WM_TIMER, 0, 0) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_wait_wakeup_signal(hcnse_msec_t ms)
{
    MSG msg;
    hcnse_memset(&msg, 0, sizeof(MSG));

    if (SetTimer(0, 0, ms, NULL) == 0) {
        return hcnse_get_errno();
    }
    while (1) {
        if (GetMessage(&msg, 0, 0, 0)) {
            if (msg.message == WM_TIMER) {
                break;
            }
        }
    }
    return HCNSE_OK;
}

#endif
