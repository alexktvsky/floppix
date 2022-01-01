#include "fpx.system.sync.mutex.h"
#include "fpx.core.log.h"

#if (FPX_POSIX)

fpx_err_t
fpx_mutex_init(fpx_mutex_t *mutex, fpx_bitmask_t params)
{
    pthread_mutexattr_t attr;
    fpx_err_t err;

    if (pthread_mutexattr_init(&attr) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "pthread_mutexattr_init() failed");
        return err;
    }

    if (fpx_bit_is_set(params, FPX_MUTEX_SHARED)) {
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_PROCESS_SHARED) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }
    else if (fpx_bit_is_set(params, FPX_MUTEX_PRIVATE)) {
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_PROCESS_PRIVATE) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }

    if (fpx_bit_is_set(params, FPX_MUTEX_RECURSIVE)) {
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err,
                "pthread_mutexattr_setpshared(PTHREAD_MUTEX_RECURSIVE) failed");
            pthread_mutexattr_destroy(&attr);
            return err;
        }
    }

    if (pthread_mutex_init(&(mutex->handle), &attr) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_mutex_init() failed");
        pthread_mutexattr_destroy(&attr);
        return err;
    }

    pthread_mutexattr_destroy(&attr);

    return FPX_OK;
}

fpx_err_t
fpx_mutex_lock(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (pthread_mutex_lock(&(mutex->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_mutex_lock() failed");
        return err;
    }

    return FPX_OK;
}

fpx_err_t
fpx_mutex_trylock(fpx_mutex_t *mutex)
{
    if (pthread_mutex_trylock(&(mutex->handle)) != 0) {
        return FPX_BUSY;
    }
    return FPX_OK;
}

fpx_err_t
fpx_mutex_unlock(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (pthread_mutex_unlock(&(mutex->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "pthread_mutex_unlock() failed");
        return err;
    }

    return FPX_OK;
}

void
fpx_mutex_fini(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (pthread_mutex_destroy(&(mutex->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "pthread_mutex_destroy() failed");
    }
}


#elif (FPX_WIN32)


fpx_err_t
fpx_mutex_init(fpx_mutex_t *mutex, fpx_bitmask_t params)
{
    HANDLE handle;
    fpx_err_t err;

    (void) params;

    if (fpx_bit_is_set(params, FPX_MUTEX_RECURSIVE)) {
        InitializeCriticalSection(&mutex->section);
        mutex->handle = NULL;
        mutex->type = fpx_mutex_critical_section;
    }
    else {
        handle = CreateMutex(NULL, FALSE, NULL);
        if (handle == NULL) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err, "CreateMutex() failed");
            return err;
        }
        mutex->handle = handle;
        mutex->type = fpx_mutex_mutex;
    }
    
    return FPX_OK;
}

fpx_err_t
fpx_mutex_lock(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (mutex->type == fpx_mutex_critical_section) {
        EnterCriticalSection(&mutex->section);
    }
    else {
        if (WaitForSingleObject(mutex->handle, INFINITE) != WAIT_OBJECT_0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err, "WaitForSingleObject() failed");
            return err;
        }
    }

    return FPX_OK;
}

fpx_err_t
fpx_mutex_trylock(fpx_mutex_t *mutex)
{
    if (mutex->type == fpx_mutex_critical_section) {
        if (TryEnterCriticalSection(&mutex->section) == 0) {
            return FPX_BUSY;
        }
    }
    else {
        if (WaitForSingleObject(mutex->handle, 0) != WAIT_OBJECT_0) {
            return FPX_BUSY;
        }
    }

    return FPX_OK;
}

fpx_err_t
fpx_mutex_unlock(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (mutex->type == fpx_mutex_critical_section) {
        LeaveCriticalSection(&mutex->section);
    }
    else {
        if (ReleaseMutex(mutex->handle) == 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err, "ReleaseMutex() failed");
            return err;
        }
    }
    return FPX_OK;
}

void
fpx_mutex_fini(fpx_mutex_t *mutex)
{
    fpx_err_t err;

    if (mutex->type == fpx_mutex_critical_section) {
        DeleteCriticalSection(&mutex->section);
    }
    else {
        if (CloseHandle(mutex->handle) == 0) {
            err = fpx_get_errno();
            fpx_log_error1(FPX_LOG_ERROR, err, "CloseHandle() failed");
        }
    }
}

#endif
