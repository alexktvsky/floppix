#ifndef INCLUDED_HCNSE_THREAD_POOL_H
#define INCLUDED_HCNSE_THREAD_POOL_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


struct hcnse_thread_pool_s {
    hcnse_thread_task_t *tasks;
    hcnse_thread_t *threads;
    size_t n_tasks;
    size_t n_threads;
    uint32_t front;
    uint32_t rear;
    hcnse_semaphore_t sem_empty;
    hcnse_semaphore_t sem_full;
    hcnse_mutex_t mutex_deposit;
    hcnse_mutex_t mutex_fetch;
};

struct hcnse_thread_task_s {
    hcnse_thread_handler_t handler;
    void *in;
    void *out;
};


hcnse_err_t hcnse_thread_pool_create(hcnse_thread_pool_t **tp, size_t n_tasks,
    size_t n_threads);
hcnse_err_t hcnse_thread_pool_task_post(hcnse_thread_pool_t *tp,
    hcnse_thread_task_t *task);
hcnse_err_t hcnse_thread_pool_destroy(hcnse_thread_pool_t *tp);

#endif /* INCLUDED_HCNSE_THREAD_POOL_H */
