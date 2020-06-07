#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, int value, int maxval,
    uint32_t flags)
{
    (void) maxval;
    hcnse_err_t err;
    int shared = 0;

    if (flags & (HCNSE_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared flags */
        if (flags & (HCNSE_SEMAPHORE_PRIVATE)) {
            return HCNSE_FAILED;
        }
    }

    if (sem_init(&(semaphore->handler), shared, value) != 0) {
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

    if (sem_wait(&(semaphore->handler)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_wait() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (sem_trywait(&(semaphore->handler)) != 0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (sem_post(&(semaphore->handler)) != 0) {
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

    if (sem_destroy(&(semaphore->handler)) != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "sem_destroy() failed");
    }
}

#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, int value, int maxval,
    uint32_t flags)
{
    (void) flags;
    HANDLE s;
    int shared = 1;
    SECURITY_ATTRIBUTES attr;
    hcnse_err_t err;

    // if (flags & (HCNSE_SEMAPHORE_SHARED)) {
    //     shared = 1;
    //     /* Check conflict of shared flags */
    //     if (flags & (HCNSE_SEMAPHORE_PRIVATE)) {
    //         return hcnse_get_errno();
    //     }
    // }

    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = shared;

    s = CreateSemaphoreA(&attr, value, maxval, NULL);
    if (s == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CreateSemaphoreA() failed");
        return err;
    }

    semaphore->handler = s;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (WaitForSingleObject(semaphore->handler, INFINITE) != WAIT_OBJECT_0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "WaitForSingleObject() failed");
        return err;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (WaitForSingleObject(semaphore->handler, 0) != WAIT_OBJECT_0) {
        return HCNSE_BUSY;
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    hcnse_err_t err;

    if (ReleaseSemaphore(semaphore->handler, 1, NULL) == 0) {
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

    if (CloseHandle(semaphore->handler) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "CloseHandle() failed");
    }
}

#endif
