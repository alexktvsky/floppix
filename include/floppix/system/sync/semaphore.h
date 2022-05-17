#ifndef FLOPPIX_SYSTEM_SYNC_SEMAPHORE_H
#define FLOPPIX_SYSTEM_SYNC_SEMAPHORE_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"
#include "floppix/system/errno.h"
#include "floppix/util/bitmask.h"

#if (FPX_POSIX && FPX_HAVE_POSIX_SEM && !FPX_DARWIN)

struct fpx_semaphore_s {
    sem_t handle;
};

#elif (FPX_POSIX && FPX_HAVE_GCD_SEM)

struct fpx_semaphore_s {
    dispatch_semaphore_t handle;
};

#elif (FPX_WIN32)

struct fpx_semaphore_s {
    HANDLE handle;
};

#endif

/* Semaphore specific parameters */
#define FPX_SEMAPHORE_PRIVATE 0x00000001
#define FPX_SEMAPHORE_SHARED  0x00000002

typedef struct fpx_semaphore_s fpx_semaphore_t;

fpx_err_t fpx_semaphore_init(fpx_semaphore_t *semaphore, unsigned int value,
    unsigned int maxval, fpx_bitmask_t params);
fpx_err_t fpx_semaphore_wait(fpx_semaphore_t *semaphore);
fpx_err_t fpx_semaphore_trywait(fpx_semaphore_t *semaphore);
fpx_err_t fpx_semaphore_post(fpx_semaphore_t *semaphore);
void fpx_semaphore_fini(fpx_semaphore_t *semaphore);

#endif /* FLOPPIX_SYSTEM_SYNC_SEMAPHORE_H */
