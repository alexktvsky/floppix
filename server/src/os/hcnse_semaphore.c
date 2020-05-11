#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

hcnse_err_t
hcnse_semaphore_init(hcnse_semaphore_t *semaphore, int value, int maxval,
    uint32_t flags)
{
    (void) maxval;
    int shared = 0;

    if (flags & (HCNSE_SEMAPHORE_SHARED)) {
        shared = 1;
        /* Check conflict of shared flags */
        if (flags & (HCNSE_SEMAPHORE_PRIVATE)) {
            return hcnse_get_errno();
        }
    }

    if (sem_init(&(semaphore->handler), shared, value) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    if (sem_wait(&(semaphore->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (sem_trywait(&(semaphore->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    if (sem_post(&(semaphore->handler)) != 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

void
hcnse_semaphore_fini(hcnse_semaphore_t *semaphore)
{
    // if (sem_destroy(&(semaphore->handler)) != 0) {
    //     return hcnse_get_errno();
    // }
    // return HCNSE_OK;
    sem_destroy(&(semaphore->handler));
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
        return hcnse_get_errno();
    }

    semaphore->handler = s;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_wait(hcnse_semaphore_t *semaphore)
{
    if (WaitForSingleObject(semaphore->handler, INFINITE) != WAIT_OBJECT_0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore)
{
    if (WaitForSingleObject(semaphore->handler, 0) != WAIT_OBJECT_0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

hcnse_err_t
hcnse_semaphore_post(hcnse_semaphore_t *semaphore)
{
    if (ReleaseSemaphore(semaphore->handler, 1, NULL) == 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

void
hcnse_semaphore_fini(hcnse_semaphore_t *semaphore)
{
    // if (CloseHandle(semaphore->handler) == 0) {
    //     return hcnse_get_errno();
    // }
    // return HCNSE_OK;
    CloseHandle(semaphore->handler);
}

#endif
