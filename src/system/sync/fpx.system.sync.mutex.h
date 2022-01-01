#ifndef FPX_SYSTEM_SYNC_MUTEX_H
#define FPX_SYSTEM_SYNC_MUTEX_H

#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"
#include "fpx.util.bitmask.h"


/* Mutex specific parameters */
#define FPX_MUTEX_SHARED             0x00000001
#define FPX_MUTEX_PRIVATE            0x00000002
#define FPX_MUTEX_RECURSIVE          0x00000004
#define FPX_MUTEX_NONRECURSIVE       0x00000008
#define FPX_MUTEX_DEFAULT            0x00000010

typedef struct fpx_mutex_s fpx_mutex_t;

#if (FPX_POSIX)

struct fpx_mutex_s {
    pthread_mutex_t handle;
};

#elif (FPX_WIN32)

typedef enum {
    fpx_mutex_mutex,
    fpx_mutex_critical_section
} fpx_mutex_type_t;

struct fpx_mutex_s {
    HANDLE handle;
    fpx_mutex_type_t type;
    CRITICAL_SECTION section;
};

#endif

fpx_err_t fpx_mutex_init(fpx_mutex_t *mutex, fpx_bitmask_t params);
fpx_err_t fpx_mutex_lock(fpx_mutex_t *mutex);
fpx_err_t fpx_mutex_trylock(fpx_mutex_t *mutex);
fpx_err_t fpx_mutex_unlock(fpx_mutex_t *mutex);
void fpx_mutex_fini(fpx_mutex_t *mutex);

#endif /* FPX_SYSTEM_SYNC_MUTEX_H */