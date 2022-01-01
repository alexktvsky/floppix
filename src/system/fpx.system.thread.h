#ifndef FPX_SYSTEM_THREAD_H
#define FPX_SYSTEM_THREAD_H

#include "fpx.system.os.portable.h"
#include "fpx.system.type.h"
#include "fpx.system.errno.h"
#include "fpx.util.bitmask.h"

/* Thread specific parameters */
#define FPX_THREAD_SCOPE_SYSTEM           0x00000001
#define FPX_THREAD_SCOPE_PROCESS          0x00000002
#define FPX_THREAD_CREATE_DETACHED        0x00000004
#define FPX_THREAD_CREATE_JOINABLE        0x00000008

#if (FPX_POSIX)

/* TODO: Add priority for unix threads */
#define FPX_THREAD_PRIORITY_HIGHEST       0
#define FPX_THREAD_PRIORITY_ABOVE_NORMAL  0
#define FPX_THREAD_PRIORITY_NORMAL        0
#define FPX_THREAD_PRIORITY_BELOW_NORMAL  0
#define FPX_THREAD_PRIORITY_LOWEST        0

#define fpx_thread_current_handle()       pthread_self()
#define fpx_thread_equal(t1, t2)          pthread_equal(t1, t2)
#define fpx_thread_exit(retval)  pthread_exit((fpx_thread_value_t) retval)

#if (FPX_LINUX)
#define FPX_FMT_TID_T                     "%d"
#elif (FPX_FREEBSD)
#define FPX_FMT_TID_T                     "%u"
#elif (FPX_DARWIN)
#define FPX_FMT_TID_T                     "%lu"
#else
#define FPX_FMT_TID_T                     "%lu"
#endif

#elif (FPX_WIN32)

#define FPX_THREAD_PRIORITY_HIGHEST       THREAD_PRIORITY_HIGHEST
#define FPX_THREAD_PRIORITY_ABOVE_NORMAL  THREAD_PRIORITY_ABOVE_NORMAL
#define FPX_THREAD_PRIORITY_NORMAL        THREAD_PRIORITY_NORMAL
#define FPX_THREAD_PRIORITY_BELOW_NORMAL  THREAD_PRIORITY_BELOW_NORMAL
#define FPX_THREAD_PRIORITY_LOWEST        THREAD_PRIORITY_LOWEST

#define fpx_thread_current_handle()       GetCurrentThread()
#define fpx_thread_equal(t1, t2)          (t1 == t2)
#define fpx_thread_exit(retval)  ExitThread((fpx_thread_value_t) retval)

#define FPX_FMT_TID_T                     "%lu"

#endif

typedef struct fpx_thread_s fpx_thread_t;
typedef fpx_thread_value_t (*fpx_thread_function_t)(void *);

struct fpx_thread_s {
    fpx_thread_handle_t handle;

};

fpx_err_t fpx_thread_init(fpx_thread_t *thread, fpx_bitmask_t params,
    fpx_size_t stack_size, fpx_int_t prio, fpx_thread_function_t start_routine,
    void *arg);
fpx_err_t fpx_thread_join(fpx_thread_t *thread);
fpx_err_t fpx_thread_cancel(fpx_thread_t *thread);
void fpx_thread_fini(fpx_thread_t *thread);

fpx_tid_t fpx_thread_current_tid(void);

#endif /* FPX_SYSTEM_THREAD_H */
