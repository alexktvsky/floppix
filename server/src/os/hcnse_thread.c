#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_thread_init(hcnse_thread_t *thread, uint32_t flags, size_t stack_size,
    int prio, hcnse_thread_function_t start_routine, void *arg)
{
    pthread_attr_t attr;
    hcnse_err_t err;
    int done_scope = 0;
    int done_detached = 0;
    (void) prio;

    if (pthread_attr_init(&attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_attr_init() failed");
        return err;
    }

    if (flags & (HCNSE_THREAD_SCOPE_SYSTEM)) {
        done_scope = 1;
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_attr_setscope() failed");
            return err;
        }
    }
    if (flags & (HCNSE_THREAD_SCOPE_PROCESS)) {
        /* Check conflict of scope flags */
        if (done_scope) {
            return HCNSE_FAILED;
        }
        else {
            done_scope = 1;
        }
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_attr_setscope() failed");
            return err;
        }
    }

    if (flags & (HCNSE_THREAD_CREATE_DETACHED)) {
        done_detached = 1;
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_attr_setdetachstate() failed");
            return err;
        }
    }
    if (flags & (HCNSE_THREAD_CREATE_JOINABLE)) {
        /* Check conflict of detached flags */
        if (done_detached) {
            return HCNSE_FAILED;
        }
        else {
            done_detached = 1;
        }
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_attr_setdetachstate() failed");
            return err;
        }
    }

    if (stack_size) {
        if (pthread_attr_setstacksize(&attr, stack_size) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_attr_setstacksize() failed");
            return err;
        }
    }

    if (pthread_create(&(thread->handler), &attr, start_routine, arg) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_create() failed");
        return err;
    }

    hcnse_memmove(&(thread->attr), &attr, sizeof(pthread_attr_t));

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (pthread_join(thread->handler, NULL) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_join() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_cancel(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (pthread_cancel(thread->handler) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_cancel() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_thread_fini(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (pthread_attr_destroy(&(thread->attr)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_attr_destroy() failed");
    }
}


#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_thread_init(hcnse_thread_t *thread, uint32_t flags, size_t stack_size,
    int prio, hcnse_thread_function_t start_routine, void *arg)
{
    (void) flags;
    HANDLE *t;
    hcnse_err_t err;
    
    t = CreateThread(NULL, stack_size, start_routine, arg, 0, NULL);
    if (t == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateThread() failed");
        return err;
    }

    if (SetThreadPriority(t, prio) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "SetThreadPriority() failed");
        return err;
    } 

    thread->handler = t;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (WaitForSingleObject(thread->handler, INFINITE) != WAIT_OBJECT_0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_cancel(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (TerminateThread(thread->handler, 0) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "TerminateThread() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_thread_fini(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (CloseHandle(thread->handler) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
