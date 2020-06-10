#ifndef INCLUDED_HCNSE_THREAD_H
#define INCLUDED_HCNSE_THREAD_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

/* Thread specific flags */
#define HCNSE_THREAD_SCOPE_SYSTEM           0x01
#define HCNSE_THREAD_SCOPE_PROCESS          0x02
#define HCNSE_THREAD_CREATE_DETACHED        0x04
#define HCNSE_THREAD_CREATE_JOINABLE        0x08


#if (HCNSE_POSIX)

#define HCNSE_THREAD_PRIORITY_HIGHEST       0
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL  0
#define HCNSE_THREAD_PRIORITY_NORMAL        0
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL  0
#define HCNSE_THREAD_PRIORITY_LOWEST        0

#define hcnse_thread_exit(retval)  pthread_exit((hcnse_thread_value_t) retval)

struct hcnse_thread_s {
    pthread_t handler;
    pthread_attr_t attr;
};

#elif (HCNSE_WIN32)

#define HCNSE_THREAD_PRIORITY_HIGHEST       THREAD_PRIORITY_HIGHEST
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL  THREAD_PRIORITY_ABOVE_NORMAL
#define HCNSE_THREAD_PRIORITY_NORMAL        THREAD_PRIORITY_NORMAL
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL  THREAD_PRIORITY_BELOW_NORMAL
#define HCNSE_THREAD_PRIORITY_LOWEST        THREAD_PRIORITY_LOWEST

#define hcnse_thread_exit(retval)  ExitThread((hcnse_thread_value_t) retval)

struct hcnse_thread_s {
    HANDLE handler;
};

#endif

hcnse_err_t hcnse_thread_init(hcnse_thread_t *thread, uint32_t flags,
    size_t stack_size, int prio, hcnse_thread_function_t start_routine,
    void *arg);
hcnse_err_t hcnse_thread_join(hcnse_thread_t *thread);
hcnse_err_t hcnse_thread_cancel(hcnse_thread_t *thread);
void hcnse_thread_fini(hcnse_thread_t *thread);

#endif /* INCLUDED_HCNSE_THREAD_H */
