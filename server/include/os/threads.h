#ifndef INCLUDED_THREADS_H
#define INCLUDED_THREADS_H

#include <stdint.h>
#include <stddef.h>

#include "server/errors.h"

/* Thread specific flags */
#define HCNSE_THREAD_SCOPE_SYSTEM        0x01
#define HCNSE_THREAD_SCOPE_PROCESS       0x02
#define HCNSE_THREAD_CREATE_DETACHED     0x04
#define HCNSE_THREAD_CREATE_JOINABLE     0x08

/* Mutex specific flags */
#define HCNSE_MUTEX_PROCESS_SHARED       0x01
#define HCNSE_MUTEX_PROCESS_PRIVATE      0x02


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
#include <pthread.h>

#define HCNSE_THREAD_PRIORITY_HIGHEST      0
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL 0
#define HCNSE_THREAD_PRIORITY_NORMAL       0
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL 0
#define HCNSE_THREAD_PRIORITY_LOWEST       0

#define hcnse_thread_exit(retval) pthread_exit((hcnse_thread_value_t) retval)

typedef void * hcnse_thread_value_t;

typedef struct {
    pthread_t handler;
    pthread_attr_t attr;
} hcnse_thread_t;

typedef struct {
    pthread_mutex_t handler;
    pthread_mutexattr_t attr;
} hcnse_mutex_t;


#elif (HCNSE_WINDOWS)
#include <windows.h>
#include <process.h>

#define HCNSE_THREAD_PRIORITY_HIGHEST      THREAD_PRIORITY_HIGHEST
#define HCNSE_THREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL
#define HCNSE_THREAD_PRIORITY_NORMAL       THREAD_PRIORITY_NORMAL
#define HCNSE_THREAD_PRIORITY_BELOW_NORMAL THREAD_PRIORITY_BELOW_NORMAL
#define HCNSE_THREAD_PRIORITY_LOWEST       THREAD_PRIORITY_LOWEST

#define hcnse_thread_exit(retval) ExitThread((hcnse_thread_value_t) retval)

typedef DWORD hcnse_thread_value_t;


typedef struct {
    HANDLE handler;
} hcnse_thread_t;

typedef struct {
    HANDLE handler;
} hcnse_mutex_t;


#endif

typedef hcnse_thread_value_t (*hcnse_thread_function_t)(void *);

hcnse_err_t hcnse_thread_create(hcnse_thread_t *thread, uint32_t flags,
    size_t stack_size, int prio, hcnse_thread_function_t start_routine,
    void *arg);
hcnse_err_t hcnse_thread_join(hcnse_thread_t *thread);
hcnse_err_t hcnse_thread_cancel(hcnse_thread_t *thread);
hcnse_err_t hcnse_thread_destroy(hcnse_thread_t *thread);

hcnse_err_t hcnse_mutex_init(hcnse_mutex_t *mutex, uint32_t flags);
hcnse_err_t hcnse_mutex_lock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_trylock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_unlock(hcnse_mutex_t *mutex);
hcnse_err_t hcnse_mutex_fini(hcnse_mutex_t *mutex);

#endif /* INCLUDED_THREADS_H */
