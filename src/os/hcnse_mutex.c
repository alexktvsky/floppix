#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, hcnse_bitmask_t params)
{
    pthread_mutexattr_t attr;
    hcnse_uint_t done_shared;
    hcnse_err_t err;

    done_shared = 0;

    if (pthread_mutexattr_init(&attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_init() failed");
        return err;
    }

    if (hcnse_bit_is_set(params, HCNSE_MUTEX_SHARED)) {
        done_shared = 1;
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_setpshared() failed");
        return err;
        }
    }
    if (hcnse_bit_is_set(params, HCNSE_MUTEX_PRIVATE)) {
        /* Check conflict of shared params */
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

    if (pthread_mutex_init(&(mutex->handle), &attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_init() failed");
        return err;
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (pthread_mutex_lock(&(mutex->handle)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_lock() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_trylock(hcnse_mutex_t *mutex)
{
    if (pthread_mutex_trylock(&(mutex->handle)) != 0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_unlock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (pthread_mutex_unlock(&(mutex->handle)) != 0) {
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

    if (pthread_mutex_destroy(&(mutex->handle)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutex_destroy() failed");
    }
}


#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, hcnse_bitmask_t params)
{
    HANDLE m;
    hcnse_err_t err;

    (void) params;

    m = CreateMutex(NULL, FALSE, NULL);
    if (m == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateMutex() failed");
        return err;
    }
    mutex->handle = m;
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (WaitForSingleObject(mutex->handle, INFINITE) != WAIT_OBJECT_0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_trylock(hcnse_mutex_t *mutex)
{
    if (WaitForSingleObject(mutex->handle, 0) != WAIT_OBJECT_0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_unlock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (ReleaseMutex(mutex->handle) == 0) {
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

    if (CloseHandle(mutex->handle) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
