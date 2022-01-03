#include "fpx.system.thread.h"
#include "fpx.core.log.h"

#if (FPX_POSIX)

fpx_err_t
fpx_thread_init(fpx_thread_t *thread, fpx_bitmask_t params,
    fpx_size_t stack_size, fpx_int_t prio, fpx_thread_function_t start_routine,
    void *arg)
{
    pthread_attr_t attr;
    fpx_err_t err;

    (void) prio;

    if (pthread_attr_init(&attr) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_attr_init() failed");
        return err;
    }

    if (fpx_bit_is_set(params, FPX_THREAD_SCOPE_SYSTEM)) {
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_attr_setscope() failed");
            return err;
        }
    }
    else if (fpx_bit_is_set(params, FPX_THREAD_SCOPE_PROCESS)) {
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_attr_setscope() failed");
            return err;
        }
    }

    if (fpx_bit_is_set(params, FPX_THREAD_CREATE_DETACHED)) {
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_attr_setdetachstate() failed");
            return err;
        }
    }
    else if (fpx_bit_is_set(params, FPX_THREAD_CREATE_JOINABLE)) {
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_attr_setdetachstate() failed");
            return err;
        }
    }

    if (stack_size) {
        if (pthread_attr_setstacksize(&attr, stack_size) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_attr_setstacksize() failed");
            return err;
        }
    }

    if (pthread_create(&(thread->handle), &attr, start_routine, arg) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_create() failed");
        return err;
    }

    if (pthread_attr_destroy(&attr) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_attr_destroy() failed");
        /* Ignore error here */
    }

    fpx_log_debug1(FPX_OK, "Create new thread");

    return FPX_OK;
}

fpx_err_t
fpx_thread_join(fpx_thread_t *thread)
{
    fpx_err_t err;

    if (pthread_join(thread->handle, NULL) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_join() failed");
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_thread_cancel(fpx_thread_t *thread)
{
    fpx_err_t err;

    if (pthread_cancel(thread->handle) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_cancel() failed");
        return err;
    }

    fpx_log_debug1(FPX_OK, "Terminate thread");

    return FPX_OK;
}

void
fpx_thread_fini(fpx_thread_t *thread)
{
    (void) thread;
}

#if (FPX_LINUX)
fpx_tid_t
fpx_thread_current_tid(void)
{
    return syscall(SYS_gettid);
}

#elif (FPX_FREEBSD)
fpx_tid_t
fpx_thread_current_tid(void)
{
    return pthread_getthreadid_np();
}

#elif (FPX_DARWIN)
fpx_tid_t
fpx_thread_current_tid(void)
{
    uint64_t tid;

    pthread_threadid_np(NULL, &tid);
    return tid;
}

#else
fpx_tid_t
fpx_thread_current_tid(void)
{
    return (uint64_t) pthread_self();
}
#endif

#elif (FPX_WIN32)

fpx_err_t
fpx_thread_init(fpx_thread_t *thread, fpx_bitmask_t params,
    fpx_size_t stack_size, fpx_int_t prio, fpx_thread_function_t start_routine,
    void *arg)
{
    HANDLE *t;
    fpx_err_t err;

    (void) params;

    t = CreateThread(NULL, stack_size, start_routine, arg, 0, NULL);
    if (t == NULL) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "CreateThread() failed");
        return err;
    }

    if (SetThreadPriority(t, prio) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "SetThreadPriority() failed");
        return err;
    }

    thread->handle = t;

    fpx_log_debug1(FPX_OK, "Create new thread");

    return FPX_OK;
}

fpx_err_t
fpx_thread_join(fpx_thread_t *thread)
{
    fpx_err_t err;

    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_thread_cancel(fpx_thread_t *thread)
{
    fpx_err_t err;

    if (TerminateThread(thread->handle, 0) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "TerminateThread() failed");
        return err;
    }

    fpx_log_debug1(FPX_OK, "Terminate thread");

    return FPX_OK;
}

void
fpx_thread_fini(fpx_thread_t *thread)
{
    fpx_err_t err;

    if (CloseHandle(thread->handle) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "CloseHandle() failed");
    }
}

fpx_tid_t
fpx_thread_current_tid(void)
{
    return GetCurrentThreadId();
}

#endif
