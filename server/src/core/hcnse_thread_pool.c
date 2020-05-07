#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_thread_value_t
hcnse_thread_routine(void *arg)
{
    hcnse_msleep(200);
    hcnse_thread_pool_t *tp = (hcnse_thread_pool_t *) arg;
    hcnse_thread_task_t *tasks = tp->tasks;
    hcnse_thread_task_t *task;
    hcnse_thread_handler_t handler;

    while (1) {
        hcnse_semaphore_wait(&(tp->sem_full));
        hcnse_mutex_lock(&(tp->mutex_fetch));

        task = &(tasks[tp->front]);
        tp->front = ((tp->front) + 1) % (tp->n_tasks);

        handler = task->handler;
        handler(task->in, task->out);

        hcnse_mutex_unlock(&(tp->mutex_fetch));
        hcnse_semaphore_post(&(tp->sem_empty));
    }
    hcnse_thread_exit(0);
}

hcnse_err_t
hcnse_thread_pool_create(hcnse_thread_pool_t **tp, size_t n_tasks,
    size_t n_threads)
{
    hcnse_err_t err;
    hcnse_thread_t *thread;

    hcnse_thread_pool_t *tp1 = hcnse_malloc(sizeof(hcnse_thread_pool_t));
    if (!tp1) {
        return hcnse_get_errno();
    }

    hcnse_thread_task_t *tasks = hcnse_malloc(sizeof(hcnse_thread_task_t) *
        n_tasks);
    if (!tasks) {
        return hcnse_get_errno();
    }

    hcnse_thread_t *threads = hcnse_malloc(sizeof(hcnse_thread_t) * n_threads);
    if (!threads) {
        return hcnse_get_errno();
    }

    err = hcnse_semaphore_init(&(tp1->sem_empty), n_tasks, n_tasks,
        HCNSE_SEMAPHORE_SHARED);
    if (err) {
        return hcnse_get_errno();
    }

    err = hcnse_semaphore_init(&(tp1->sem_full), 0, n_tasks,
        HCNSE_SEMAPHORE_SHARED);
    if (err) {
        return hcnse_get_errno();
    }

    err = hcnse_mutex_init(&(tp1->mutex_deposit), HCNSE_MUTEX_SHARED);
    if (err) {
        return hcnse_get_errno();
    }

    err = hcnse_mutex_init(&(tp1->mutex_fetch), HCNSE_MUTEX_SHARED);
    if (err) {
        return hcnse_get_errno();
    }

    for (size_t i = 0; i < n_threads; i++) {
        thread = &(threads[i]);
        hcnse_thread_init(thread,
            HCNSE_THREAD_SCOPE_SYSTEM|HCNSE_THREAD_CREATE_DETACHED,
            0, HCNSE_THREAD_PRIORITY_NORMAL,
            hcnse_thread_routine, (void *) tp1);
    }

    tp1->tasks = tasks;
    tp1->threads = threads;
    tp1->n_tasks = n_tasks;
    tp1->n_threads = n_threads;

    *tp = tp1;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_thread_pool_destroy(hcnse_thread_pool_t *tp)
{
    hcnse_thread_t *thread;

    for (size_t i = 0; i < tp->n_threads; i++) {
        thread = &((tp->threads)[i]);
        hcnse_thread_cancel(thread);
        hcnse_msleep(100);
        hcnse_thread_fini(thread);
    }
    hcnse_free(tp->tasks);
    hcnse_free(tp->threads);
    hcnse_free(tp);
    return HCNSE_OK;
}


hcnse_err_t
hcnse_thread_task_post(hcnse_thread_pool_t *tp, hcnse_thread_task_t *task)
{
    hcnse_thread_task_t *tasks = tp->tasks;

    hcnse_semaphore_wait(&(tp->sem_empty));
    hcnse_mutex_lock(&(tp->mutex_deposit));

    hcnse_memmove(&(tasks[tp->rear]), task, sizeof(hcnse_thread_task_t));
    tp->rear = ((tp->rear) + 1) % (tp->n_tasks);

    hcnse_mutex_unlock(&(tp->mutex_deposit));
    hcnse_semaphore_post(&(tp->sem_full));

    return HCNSE_OK;
}
