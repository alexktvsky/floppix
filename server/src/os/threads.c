#include <string.h>

#include "os/threads.h"


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
hcnse_err_t
hcnse_thread_create(hcnse_thread_t *thread, uint32_t flags, size_t stack_size,
    int prio, hcnse_thread_function_t start_routine, void *arg)
{
    pthread_attr_t attr;
    int done_scope = 0;
    int done_detached = 0;
    (void) prio;

    if (pthread_attr_init(&attr) != 0) {
        return hcnse_get_errno();
    }

    if (flags & (HCNSE_THREAD_SCOPE_SYSTEM)) {
        done_scope = 1;
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0) {
            return hcnse_get_errno();
        }
    }
    if (flags & (HCNSE_THREAD_SCOPE_PROCESS)) {
        /* Check conflict of scope flags */
        if (done_scope) {
            return hcnse_get_errno();
        }
        else {
            done_scope = 1;
        }
        if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS) != 0) {
            return hcnse_get_errno();
        }
    }

    if (flags & (HCNSE_THREAD_CREATE_DETACHED)) {
        done_detached = 1;
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
            return hcnse_get_errno();
        }
    }
    if (flags & (HCNSE_THREAD_CREATE_JOINABLE)) {
        /* Check conflict of detached flags */
        if (done_detached) {
            return hcnse_get_errno();
        }
        else {
            done_detached = 1;
        }
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
            return hcnse_get_errno();
        }
    }

    if (stack_size) {
        if (pthread_attr_setstacksize(&attr, stack_size) != 0) {
            return hcnse_get_errno();
        }
    }

    if (pthread_create(&(thread->handler), &attr, start_routine, arg) != 0) {
        return hcnse_get_errno();
    }

    memmove(&(thread->attr), &attr, sizeof(pthread_attr_t));


    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    if (pthread_join(thread->handler, NULL) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_cancel(hcnse_thread_t *thread)
{
    if (pthread_cancel(thread->handler) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_destroy(hcnse_thread_t *thread)
{
    (void) thread;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags)
{
    pthread_mutexattr_t attr;
    int done_shared = 0;

    if (pthread_mutexattr_init(&attr) != 0) {
        return hcnse_get_errno();
    }

    if (flags & (HCNSE_MUTEX_PROCESS_SHARED)) {
        done_shared = 1;
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
            return hcnse_get_errno();
        }
    }
    if (flags & (HCNSE_MUTEX_PROCESS_PRIVATE)) {
        /* Check conflict of shared flags */
        if (done_shared) {
            return hcnse_get_errno();
        }
        else {
            done_shared = 1;
        }
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE) != 0) {
            return hcnse_get_errno();
        }
    }

    if (pthread_mutex_init(&(mutex->handler), &attr) != 0) {
        return hcnse_get_errno();
    }

    memmove(&(mutex->attr), &attr, sizeof(pthread_mutexattr_t));

    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    if (pthread_mutex_lock(&(mutex->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_trylock(hcnse_mutex_t *mutex)
{
    if (pthread_mutex_trylock(&(mutex->handler)) != 0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_unlock(hcnse_mutex_t *mutex)
{
    if (pthread_mutex_unlock(&(mutex->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_fini(hcnse_mutex_t *mutex)
{
    if (pthread_mutex_destroy(&(mutex->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}


#elif (HCNSE_WINDOWS)
hcnse_err_t
hcnse_thread_create(hcnse_thread_t *thread, uint32_t flags, size_t stack_size,
    int prio, hcnse_thread_function_t start_routine, void *arg)
{
    (void) flags;
    HANDLE *t;
    
    t = CreateThread(NULL, stack_size, start_routine, arg, 0, NULL);
    if (t == NULL) {
        return hcnse_get_errno();
    }

    if (SetThreadPriority(t, prio) == 0) {
        return hcnse_get_errno();
    } 

    thread->handler = t;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_join(hcnse_thread_t *thread)
{
    if (WaitForSingleObject(thread->handler, INFINITE) != WAIT_OBJECT_0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_cancel(hcnse_thread_t *thread)
{
    if (TerminateThread(thread->handler, 0) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_destroy(hcnse_thread_t *thread)
{
    if (CloseHandle(thread->handler) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags)
{
    (void) flags;
    HANDLE m;

    m = CreateMutex(NULL, FALSE, NULL);
    if (m == NULL) {
        return hcnse_get_errno();
    }
    mutex->handler = m;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    if (WaitForSingleObject(mutex->handler, INFINITE) != WAIT_OBJECT_0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_trylock(hcnse_mutex_t *mutex)
{
    if (WaitForSingleObject(mutex->handler, 0) != WAIT_OBJECT_0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_unlock(hcnse_mutex_t *mutex)
{
    if (ReleaseMutex(mutex->handler) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_fini(hcnse_mutex_t *mutex)
{
    if (CloseHandle(mutex->handler) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

#endif
