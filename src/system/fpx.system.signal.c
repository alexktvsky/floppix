#include "fpx.system.signal.h"

#if (FPX_POSIX)
fpx_err_t
fpx_send_wakeup_signal(fpx_thread_t *thread)
{
    if (pthread_kill(thread->handle, SIGALRM) != 0) {
        return fpx_get_errno();
    }
    return FPX_OK;
}

fpx_err_t
fpx_wait_wakeup_signal(fpx_msec_t ms)
{
    if (usleep(ms * 1000) != 0) {
        return fpx_get_errno();
    }
    return FPX_OK;
}

#elif (FPX_WIN32)
fpx_err_t
fpx_send_wakeup_signal(fpx_thread_t *thread)
{
    if (PostThreadMessageA(GetThreadId(thread->handle), WM_TIMER, 0, 0) == 0) {
        return fpx_get_errno();
    }
    return FPX_OK;
}

fpx_err_t
fpx_wait_wakeup_signal(fpx_msec_t ms)
{
    MSG msg;
    fpx_memset(&msg, 0, sizeof(MSG));

    if (SetTimer(0, 0, ms, NULL) == 0) {
        return fpx_get_errno();
    }
    while (1) {
        if (GetMessage(&msg, 0, 0, 0)) {
            if (msg.message == WM_TIMER) {
                break;
            }
        }
    }
    return FPX_OK;
}

#endif
