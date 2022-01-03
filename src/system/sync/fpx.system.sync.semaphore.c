#include "fpx.system.sync.semaphore.h"
#include "fpx.core.log.h"

#if (FPX_POSIX && FPX_HAVE_POSIX_SEM && !FPX_DARWIN)

fpx_err_t
fpx_semaphore_init(fpx_semaphore_t *semaphore, fpx_uint_t value,
    fpx_uint_t maxval, fpx_bitmask_t params)
{
    fpx_uint_t shared;
    fpx_err_t err;

    (void) maxval;
    shared = 0;

    if (fpx_bit_is_set(params, FPX_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared params */
        if (params & (FPX_SEMAPHORE_PRIVATE)) {
            return FPX_FAILED;
        }
    }

    if (sem_init(&(semaphore->handle), shared, value) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "sem_init() failed");
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_wait(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (sem_wait(&(semaphore->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "sem_wait() failed");
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_trywait(fpx_semaphore_t *semaphore)
{
    if (sem_trywait(&(semaphore->handle)) != 0) {
        return FPX_BUSY;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_post(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (sem_post(&(semaphore->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "sem_post() failed");
        return err;
    }
    return FPX_OK;
}

void
fpx_semaphore_fini(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (sem_destroy(&(semaphore->handle)) != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "sem_destroy() failed");
    }
}

#elif (FPX_POSIX && FPX_HAVE_GCD_SEM)

fpx_err_t
fpx_semaphore_init(fpx_semaphore_t *semaphore, fpx_uint_t value,
    fpx_uint_t maxval, fpx_bitmask_t params)
{
    fpx_err_t err;
    dispatch_semaphore_t s;

    (void) params;
    (void) maxval;

    s = dispatch_semaphore_create(value);
    if (!s) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "dispatch_semaphore_create() failed");
        return err;
    }

    semaphore->handle = s;

    return FPX_OK;
}

fpx_err_t
fpx_semaphore_wait(fpx_semaphore_t *semaphore)
{
    dispatch_semaphore_wait(semaphore->handle, DISPATCH_TIME_FOREVER);
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_trywait(fpx_semaphore_t *semaphore)
{
    if (dispatch_semaphore_wait(semaphore->handle, DISPATCH_TIME_NOW) != 0) {
        return FPX_BUSY;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_post(fpx_semaphore_t *semaphore)
{
    dispatch_semaphore_signal(semaphore->handle);
    return FPX_OK;
}

void
fpx_semaphore_fini(fpx_semaphore_t *semaphore)
{
    (void) semaphore;
}

#elif (FPX_WIN32)

fpx_err_t
fpx_semaphore_init(fpx_semaphore_t *semaphore, fpx_uint_t value,
    fpx_uint_t maxval, fpx_bitmask_t params)
{
    HANDLE s;
    SECURITY_ATTRIBUTES attr;
    fpx_uint_t shared;
    fpx_err_t err;

    (void) params;

    shared = 1;

#if 0
    if (fpx_bit_is_set(params, FPX_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared params */
        if (params & (FPX_SEMAPHORE_PRIVATE)) {
            return fpx_get_errno();
        }
    }
#endif

    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = shared;

    s = CreateSemaphoreA(&attr, value, maxval, NULL);
    if (s == NULL) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "CreateSemaphoreA() failed");
        return err;
    }

    semaphore->handle = s;

    return FPX_OK;
}

fpx_err_t
fpx_semaphore_wait(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (WaitForSingleObject(semaphore->handle, INFINITE) != WAIT_OBJECT_0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_trywait(fpx_semaphore_t *semaphore)
{
    if (WaitForSingleObject(semaphore->handle, 0) != WAIT_OBJECT_0) {
        return FPX_BUSY;
    }
    return FPX_OK;
}

fpx_err_t
fpx_semaphore_post(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (ReleaseSemaphore(semaphore->handle, 1, NULL) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "ReleaseSemaphore() failed");
        return err;
    }
    return FPX_OK;
}

void
fpx_semaphore_fini(fpx_semaphore_t *semaphore)
{
    fpx_err_t err;

    if (CloseHandle(semaphore->handle) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
