#ifndef INCLUDED_HCNSE_SEMAPHORE_H
#define INCLUDED_HCNSE_SEMAPHORE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Semaphore specific flags */
#define HCNSE_SEMAPHORE_PRIVATE        0x01
#define HCNSE_SEMAPHORE_SHARED         0x02

#if (HCNSE_UNIX)

typedef struct {
    sem_t handler;
} hcnse_semaphore_t;

#elif (HCNSE_WINDOWS)

typedef struct {
    HANDLE handler;
} hcnse_semaphore_t;

#endif

int hcnse_semaphore_init(hcnse_semaphore_t *semaphore, int value, int maxval,
    uint32_t flags);
int hcnse_semaphore_wait(hcnse_semaphore_t *semaphore);
int hcnse_semaphore_trywait(hcnse_semaphore_t *semaphore);
int hcnse_semaphore_post(hcnse_semaphore_t *semaphore);
int hcnse_semaphore_fini(hcnse_semaphore_t *semaphore);

#endif /* INCLUDED_HCNSE_SEMAPHORE_H */
