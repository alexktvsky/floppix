#include "os/signals.h"

#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
#include <unistd.h>
#include <pthread.h>

#elif (HCNSE_WINDOWS)
#include <string.h>
#include <windows.h>
#include <process.h>
#endif


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
hcnse_err_t hcnse_send_wakeup_signal(hcnse_thread_t *tid)
{
    if (pthread_kill(tid->handler, SIGALRM) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_send_wakeup_signal1(pid_t pid)
{
    if (kill(pid, SIGALRM) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_wait_wakeup_signal(hcnse_msec_t ms)
{
    if (usleep(ms * 1000) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

#elif (HCNSE_WINDOWS)
hcnse_err_t hcnse_send_wakeup_signal(hcnse_thread_t *tid)
{
    if (PostThreadMessageA(GetThreadId(tid->handler), WM_TIMER, 0, 0) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t hcnse_wait_wakeup_signal(hcnse_msec_t ms)
{
    MSG msg;
    memset(&msg, 0, sizeof(MSG));

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
