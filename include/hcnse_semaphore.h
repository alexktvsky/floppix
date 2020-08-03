#ifndef INCLUDED_HCNSE_SEMAPHORE_H
#define INCLUDED_HCNSE_SEMAPHORE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Semaphore specific flags */
#define HCNSE_SEMAPHORE_PRIVATE        0x00000001
#define HCNSE_SEMAPHORE_SHARED         0x00000002

#if (HCNSE_POSIX)

struct hcnse_semaphore_s {
    sem_t handle;
};

#elif (HCNSE_WIN32)

struct hcnse_semaphore_s {
    HANDLE handle;
};

#endif

hcnse_err_t hcnse_semaphore_init(hcnse_semaphore_t *semaphore,
    hcnse_uint_t value, hcnse_uint_t maxval, hcnse_flag_t flags);
hcnse_err_t hcnse_semaphore_wait(hcnse_semaphore_t *semaphore);
hcnse_err_t hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore);
hcnse_err_t hcnse_semaphore_post(hcnse_semaphore_t *semaphore);
void hcnse_semaphore_fini(hcnse_semaphore_t *semaphore);

#endif /* INCLUDED_HCNSE_SEMAPHORE_H */
