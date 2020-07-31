#include "hcnse_portable.h"
#include "hcnse_common.h"


static hcnse_uint_t hcnse_thread_counter_value;

#if (HCNSE_POSIX)

hcnse_err_t
hcnse_thread_init(hcnse_thread_t *thread, hcnse_flag_t flags, size_t stack_size,
    hcnse_int_t prio, hcnse_thread_function_t start_routine, void *arg)
{
    static hcnse_uint_t thread_index = 1;
    pthread_attr_t attr;
    hcnse_uint_t done_scope, done_detached;
    hcnse_err_t err;

    done_scope = 0;
    done_detached = 0;
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
        done_scope = 1;

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
        done_detached = 1;

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

    if (pthread_create(&(thread->handle), &attr, start_routine, arg) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_create() failed");
        return err;
    }

    if (pthread_attr_destroy(&attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_attr_destroy() failed");
        /* Ignore error here */
    }

    thread->index = thread_index;
    thread_index += 1;

    hcnse_thread_counter_value += 1;

    hcnse_log_debug1(HCNSE_OK, "Create thread %zu, total %zu",
        thread->index, hcnse_thread_counter_value);

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (pthread_join(thread->handle, NULL) != 0) {
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

    if (pthread_cancel(thread->handle) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_cancel() failed");
        return err;
    }

    hcnse_thread_counter_value -= 1;

    hcnse_log_debug1(HCNSE_OK, "Terminate thread %zu, total %zu",
        thread->index, hcnse_thread_counter_value);

    return HCNSE_OK;
}

void
hcnse_thread_fini(hcnse_thread_t *thread)
{
    (void) thread;
}

#if (HCNSE_LINUX)
hcnse_tid_t
hcnse_thread_current_tid(void)
{
    return syscall(SYS_gettid);
}

#elif (HCNSE_FREEBSD)
hcnse_tid_t
hcnse_thread_current_tid(void)
{
    return pthread_getthreadid_np();
}

#elif (HCNSE_DARWIN)
hcnse_tid_t
hcnse_thread_current_tid(void)
{
    uint64_t tid;

    pthread_threadid_np(NULL, &tid);
    return tid;
}

#else
hcnse_tid_t
hcnse_thread_current_tid(void)
{
    return (uint64_t) pthread_self();
}
#endif


#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_thread_init(hcnse_thread_t *thread, hcnse_flag_t flags, size_t stack_size,
    hcnse_int_t prio, hcnse_thread_function_t start_routine, void *arg)
{
    static hcnse_uint_t thread_index = 1;
    HANDLE *t;
    hcnse_err_t err;

    (void) flags;

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

    thread->handle = t;

    thread->index = thread_index;
    thread_index += 1;

    hcnse_thread_counter_value += 1;

    hcnse_log_debug1(HCNSE_OK, "Create thread %zu, total %zu",
        thread->index, hcnse_thread_counter_value);

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
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

    if (TerminateThread(thread->handle, 0) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "TerminateThread() failed");
        return err;
    }

    hcnse_thread_counter_value -= 1;

    hcnse_log_debug1(HCNSE_OK, "Terminate thread %zu, total %zu",
        thread->index, hcnse_thread_counter_value);

    return HCNSE_OK;
}

void
hcnse_thread_fini(hcnse_thread_t *thread)
{
    hcnse_err_t err;

    if (CloseHandle(thread->handle) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

hcnse_tid_t
hcnse_thread_current_tid(void)
{
    return GetCurrentThreadId();
}

#endif


hcnse_uint_t
hcnse_get_thread_counter(void)
{
    return hcnse_thread_counter_value;
}
