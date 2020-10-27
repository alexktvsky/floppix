#ifndef INCLUDED_HCNSE_MUTEX_H
#define INCLUDED_HCNSE_MUTEX_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Mutex specific parameters */
#define HCNSE_MUTEX_SHARED             0x00000001
#define HCNSE_MUTEX_PRIVATE            0x00000002
#define HCNSE_MUTEX_RECURSIVE          0x00000004
#define HCNSE_MUTEX_NONRECURSIVE       0x00000008
#define HCNSE_MUTEX_DEFAULT            0x00000010


#if (HCNSE_POSIX)

struct hcnse_mutex_s {
    pthread_mutex_t handle;



};

#elif (HCNSE_WIN32)

typedef enum {
    hcnse_mutex_mutex,
    hcnse_mutex_critical_section
} hcnse_mutex_type_t;

struct hcnse_mutex_s {
    HANDLE handle;
    hcnse_mutex_type_t type;
    CRITICAL_SECTION section;
};

#endif

hcnse_err_t hcnse_mutex_init(hcnse_mutex_t *mutex, hcnse_bitfield_t params);
hcnse_err_t hcnse_mutex_lock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_trylock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_unlock(hcnse_mutex_t *mutex);
void hcnse_mutex_fini(hcnse_mutex_t *mutex);

#endif /* INCLUDED_HCNSE_MUTEX_H */