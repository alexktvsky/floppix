#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_mutex_init(hcnse_mutex_t *mutex, hcnse_bitfield_t params)
{
    pthread_mutexattr_t attr;
    hcnse_err_t err;

    if (pthread_mutexattr_init(&attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "pthread_mutexattr_init() failed");
        return err;
    }

    if (hcnse_bit_is_set(params, HCNSE_MUTEX_SHARED)) {
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_PROCESS_SHARED) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }
    else if (hcnse_bit_is_set(params, HCNSE_MUTEX_PRIVATE)) {
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_PROCESS_PRIVATE) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }

    if (hcnse_bit_is_set(params, HCNSE_MUTEX_RECURSIVE)) {
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_MUTEX_RECURSIVE) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }

    if (pthread_mutex_init(&(mutex->handle), &attr) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "pthread_mutex_init() failed");
        pthread_mutexattr_destroy(&attr);
        return err;
    }

    pthread_mutexattr_destroy(&attr);

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
hcnse_mutex_init(hcnse_mutex_t *mutex, hcnse_bitfield_t params)
{
    HANDLE handle;
    hcnse_err_t err;

    (void) params;

    if (hcnse_bit_is_set(params, HCNSE_MUTEX_RECURSIVE)) {
        InitializeCriticalSection(&mutex->section);
        mutex->handle = NULL;
        mutex->type = hcnse_mutex_critical_section;
    }
    else {
        handle = CreateMutex(NULL, FALSE, NULL);
        if (handle == NULL) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateMutex() failed");
            return err;
        }
        mutex->handle = handle;
        mutex->type = hcnse_mutex_mutex;
    }
    
    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_lock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (mutex->type == hcnse_mutex_critical_section) {
        EnterCriticalSection(&mutex->section);
    }
    else {
        if (WaitForSingleObject(mutex->handle, INFINITE) != WAIT_OBJECT_0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
            return err;
        }
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_trylock(hcnse_mutex_t *mutex)
{
    if (mutex->type == hcnse_mutex_critical_section) {
        if (TryEnterCriticalSection(&mutex->section) == 0) {
            return HCNSE_BUSY;
        }
    }
    else {
        if (WaitForSingleObject(mutex->handle, 0) != WAIT_OBJECT_0) {
            return HCNSE_BUSY;
        }
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_mutex_unlock(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (mutex->type == hcnse_mutex_critical_section) {
        LeaveCriticalSection(&mutex->section);
    }
    else {
        if (ReleaseMutex(mutex->handle) == 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "ReleaseMutex() failed");
            return err;
        }
    }
    return HCNSE_OK;
}

void
hcnse_mutex_fini(hcnse_mutex_t *mutex)
{
    hcnse_err_t err;

    if (mutex->type == hcnse_mutex_critical_section) {
        DeleteCriticalSection(&mutex->section);
    }
    else {
        if (CloseHandle(mutex->handle) == 0) {
            err = hcnse_get_errno();
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
        }
    }
}

#endif
