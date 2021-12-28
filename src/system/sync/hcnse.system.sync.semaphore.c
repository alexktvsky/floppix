#include "hcnse.system.sync.semaphore.h"
#include "hcnse.core.log.h"


#if (HCNSE_POSIX && HCNSE_HAVE_POSIX_SEM && !HCNSE_DARWIN)

hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, hcnse_uint_t value,
    hcnse_uint_t maxval, hcnse_bitfield_t params)
{
    hcnse_uint_t shared;
    hcnse_err_t err;

    (void) maxval;
    shared = 0;

    if (hcnse_bit_is_set(params, HCNSE_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared params */
        if (params & (HCNSE_SEMAPHORE_PRIVATE)) {
            return HCNSE_FAILED;
        }
    }

    if (sem_init(&(semaphore->handle), shared, value) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_init() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (sem_wait(&(semaphore->handle)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_wait() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (sem_trywait(&(semaphore->handle)) != 0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (sem_post(&(semaphore->handle)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_post() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_semaphore_fini(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (sem_destroy(&(semaphore->handle)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_destroy() failed");
    }
}


#elif (HCNSE_POSIX && HCNSE_HAVE_GCD_SEM)

hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, hcnse_uint_t value,
    hcnse_uint_t maxval, hcnse_bitfield_t params)
{
    hcnse_err_t err;
    dispatch_semaphore_t s;

    (void) params;
    (void) maxval;

    s = dispatch_semaphore_create(value);
    if (!s) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "dispatch_semaphore_create() failed");
        return err;
    }

    semaphore->handle = s;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    dispatch_semaphore_wait(semaphore->handle, DISPATCH_TIME_FOREVER);
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (dispatch_semaphore_wait(semaphore->handle, DISPATCH_TIME_NOW) != 0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    dispatch_semaphore_signal(semaphore->handle);
    return HCNSE_OK;
}

void
hcnse_semaphore_fini(hcnse_semaphore_t *semaphore)
{
    (void) semaphore;
}


#elif (HCNSE_WIN32)


hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, hcnse_uint_t value,
    hcnse_uint_t maxval, hcnse_bitfield_t params)
{
    HANDLE s;
    SECURITY_ATTRIBUTES attr;
    hcnse_uint_t shared;
    hcnse_err_t err;

    (void) params;

    shared = 1;

#if 0
    if (hcnse_bit_is_set(params, HCNSE_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared params */
        if (params & (HCNSE_SEMAPHORE_PRIVATE)) {
            return hcnse_get_errno();
        }
    }
#endif

    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = shared;

    s = CreateSemaphoreA(&attr, value, maxval, NULL);
    if (s == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateSemaphoreA() failed");
        return err;
    }

    semaphore->handle = s;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (WaitForSingleObject(semaphore->handle, INFINITE) != WAIT_OBJECT_0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (WaitForSingleObject(semaphore->handle, 0) != WAIT_OBJECT_0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (ReleaseSemaphore(semaphore->handle, 1, NULL) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "ReleaseSemaphore() failed");
        return err;
    }
    return HCNSE_OK;
}

void
hcnse_semaphore_fini(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (CloseHandle(semaphore->handle) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
