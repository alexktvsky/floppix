#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags)
{
    pthread_mutexattr_t attr;
    int done_shared = 0;
    hcnse_err_t err;

    if (pthread_mutexattr_init(&attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_init() failed");
        return err;
    }

    if (flags & (HCNSE_MUTEX_SHARED)) {
        done_shared = 1;
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_setpshared() failed");
        return err;
        }
    }
    if (flags & (HCNSE_MUTEX_PRIVATE)) {
        /* Check conflict of shared flags */
        if (done_shared) {
            return HCNSE_FAILED;
        }
        else {
            done_shared = 1;
        }
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_setpshared() failed");
        return err;
        }
    }

    if (pthread_mutex_init(&(mutex->handler), &attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_init() failed");
        return err;
    }

    hcnse_memmove(&(mutex->attr), &attr, sizeof(pthread_mutexattr_t));

    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (pthread_mutex_lock(&(mutex->handler)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_lock() failed");
        return err;
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
    hcnse_err_t err;

    if (pthread_mutex_unlock(&(mutex->handler)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_unlock() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_mutex_fini(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (pthread_mutex_destroy(&(mutex->handler)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutex_destroy() failed");
    }
}


#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags)
{
    (void) flags;
    HANDLE m;
    hcnse_err_t err;

    m = CreateMutex(NULL, FALSE, NULL);
    if (m == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateMutex() failed");
        return err;
    }
    mutex->handler = m;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (WaitForSingleObject(mutex->handler, INFINITE) != WAIT_OBJECT_0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
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
    hcnse_err_t err;

    if (ReleaseMutex(mutex->handler) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "ReleaseMutex() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_mutex_fini(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (CloseHandle(mutex->handler) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
