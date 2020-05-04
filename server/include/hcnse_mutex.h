#ifndef INCLUDED_HCNSE_MUTEX_H
#define INCLUDED_HCNSE_MUTEX_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Mutex specific flags */
#define HCNSE_MUTEX_SHARED             0x01
#define HCNSE_MUTEX_PRIVATE            0x02


#if (HCNSE_POSIX)

typedef struct {
    pthread_mutex_t handler;
    pthread_mutexattr_t attr;
} hcnse_mutex_t;


#elif (HCNSE_WIN32)

typedef struct {
    HANDLE handler;
} hcnse_mutex_t;

#endif

hcnse_err_t hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags);
hcnse_err_t hcnse_mutex_lock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_trylock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_unlock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_fini(hcnse_mutex_t *mutex);

#endif /* INCLUDED_HCNSE_MUTEX_H */