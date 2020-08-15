#ifndef INCLUDED_HCNSE_THREAD_H
#define INCLUDED_HCNSE_THREAD_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Thread specific parameters */
#define HCNSE_THREAD_SCOPE_SYSTEM           0x00000001
#define HCNSE_THREAD_SCOPE_PROCESS          0x00000002
#define HCNSE_THREAD_CREATE_DETACHED        0x00000004
#define HCNSE_THREAD_CREATE_JOINABLE        0x00000008

#if (HCNSE_POSIX)

#define HCNSE_THREAD_PRIORITY_HIGHEST       0
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL  0
#define HCNSE_THREAD_PRIORITY_NORMAL        0
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL  0
#define HCNSE_THREAD_PRIORITY_LOWEST        0

#define hcnse_thread_current_handle()       pthread_self()
#define hcnse_thread_exit(retval)  pthread_exit((hcnse_thread_value_t) retval)

#if (HCNSE_LINUX)
#define HCNSE_FMT_TID_T                     "%d"
#elif (HCNSE_FREEBSD)
#define HCNSE_FMT_TID_T                     "%u"
#elif (HCNSE_DARWIN)
#define HCNSE_FMT_TID_T                     "%lu"
#else
#define HCNSE_FMT_TID_T                     "%lu"
#endif

#elif (HCNSE_WIN32)

#define HCNSE_THREAD_PRIORITY_HIGHEST       THREAD_PRIORITY_HIGHEST
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL  THREAD_PRIORITY_ABOVE_NORMAL
#define HCNSE_THREAD_PRIORITY_NORMAL        THREAD_PRIORITY_NORMAL
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL  THREAD_PRIORITY_BELOW_NORMAL
#define HCNSE_THREAD_PRIORITY_LOWEST        THREAD_PRIORITY_LOWEST

#define hcnse_thread_current_handle()       GetCurrentThread()
#define hcnse_thread_exit(retval)  ExitThread((hcnse_thread_value_t) retval)

#define HCNSE_FMT_TID_T                     "%lu"

#endif

struct hcnse_thread_s {
    hcnse_thread_handle_t handle;
    hcnse_uint_t index;
};


hcnse_err_t hcnse_thread_init(hcnse_thread_t *thread, hcnse_bitmask_t params,
    size_t stack_size, hcnse_int_t prio, hcnse_thread_function_t start_routine,
    void *arg);
hcnse_err_t hcnse_thread_join(hcnse_thread_t *thread);
hcnse_err_t hcnse_thread_cancel(hcnse_thread_t *thread);
void hcnse_thread_fini(hcnse_thread_t *thread);

hcnse_tid_t hcnse_thread_current_tid(void);

hcnse_uint_t hcnse_get_thread_counter(void);

#endif /* INCLUDED_HCNSE_THREAD_H */
