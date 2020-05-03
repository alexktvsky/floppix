#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LOG_MSG_SIZE             300
#define HCNSE_LOG_BUF_SIZE             32

#define HCNSE_LOG_INIT_DELAY           500
#define HCNSE_LOG_WORKER_DELAY         1000


typedef struct {
    uint8_t level;
    char time[HCNSE_TIMESTRLEN];
    char str[HCNSE_LOG_MSG_SIZE];
} hcnse_log_message_t;

struct hcnse_log_s {
    hcnse_file_t *file;
    uint8_t level;
    size_t size;
    hcnse_log_message_t *messages;
#if (HCNSE_UNIX && HCNSE_HAVE_MMAP)
    pid_t pid;
#else
    hcnse_thread_t *tid;
#endif
    uint32_t front;
    uint32_t rear;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
};

static const char *prio[] = {
    "emerg",
    "error",
    "warning",
    "info",
    "debug"
};


static hcnse_thread_value_t
hcnse_log_worker(void *arg)
{
    hcnse_log_t *log = (hcnse_log_t *) arg;
    hcnse_log_message_t *messages = log->messages;
    hcnse_log_message_t *msg;
    size_t maxlen = HCNSE_TIMESTRLEN + HCNSE_LOG_MSG_SIZE + sizeof(prio[0]);
    size_t len;
    char buf[maxlen];

    hcnse_msleep(HCNSE_LOG_WORKER_DELAY);

    while (1) {
        hcnse_semaphore_wait(log->sem_full);

        hcnse_mutex_lock(log->mutex_fetch);

        msg = &(messages[log->front]);
        log->front = ((log->front) + 1) % HCNSE_LOG_BUF_SIZE;


        len = snprintf(buf, maxlen, "%s [%s] %s\n", 
                                msg->time, prio[msg->level], msg->str);

        /* TODO: Improve logs rotation */
        if (log->size) {
            if ((log->file->offset + len) > (log->size)) {
                log->file->offset = 0;
            }
        }

        if (hcnse_file_write1(log->file, buf, len) == -1) {
            /* What need to do? */
        }

#if (HCNSE_NO_DAEMON)
        fprintf(stdout, "%s", buf);
#endif

        hcnse_mutex_unlock(log->mutex_fetch);
        hcnse_semaphore_post(log->sem_empty);
    }
    return 0;
}

/* Version for UNIX-like systems with mmap */
#if (HCNSE_UNIX && HCNSE_HAVE_MMAP)
hcnse_err_t
hcnse_log_init(hcnse_log_t **in_log, const char *fname, uint8_t level, size_t size)
{
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    void *ptr;

    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    hcnse_log_message_t *messages = MAP_FAILED;
    size_t mem_size = 0;
    ssize_t file_size;
    pid_t pid;
    hcnse_err_t err;

    log = hcnse_malloc(sizeof(hcnse_log_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }
    memset(log, 0, sizeof(hcnse_log_t));

    file = hcnse_malloc(sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, fname, HCNSE_FILE_WRONLY,
                                    HCNSE_FILE_CREATE_OR_OPEN,
                                            HCNSE_FILE_OWNER_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }
    if (((size_t) file_size) >= size) {
        err = HCNSE_ERR_LOG_BIG;
        goto failed;
    }
    file->offset = file_size;

    mem_size += sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE;
    mem_size += sizeof(hcnse_mutex_t) * 2;
    mem_size += sizeof(hcnse_semaphore_t) * 2;

    messages = mmap(NULL, mem_size, PROT_READ|PROT_WRITE,
                                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (messages == MAP_FAILED) {
        err = hcnse_get_errno();
        goto failed;
    }

    ptr = messages;
    ptr += sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE;

    mutex_deposit = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    mutex_fetch = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    sem_empty = (hcnse_semaphore_t *) ptr;
    ptr += sizeof(hcnse_semaphore_t);

    sem_full = (hcnse_semaphore_t *) ptr;

    err = hcnse_semaphore_init(sem_empty, HCNSE_LOG_BUF_SIZE,
                            HCNSE_LOG_BUF_SIZE, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_semaphore_init(sem_full, 0,
                            HCNSE_LOG_BUF_SIZE, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_mutex_init(mutex_deposit, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_mutex_init(mutex_fetch, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    /* XXX: Init all log struct fields before run worker */
    log->file = file;
    log->level = level;
    log->size = size;
    log->messages = messages;
    log->mutex_deposit = mutex_deposit;
    log->mutex_fetch = mutex_fetch;
    log->sem_empty = sem_empty;
    log->sem_full = sem_full;

    pid = fork();
    if (pid == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    if (!pid) {
        hcnse_log_worker(log);
    }
    else {
        log->pid = pid;
    }

    *in_log = log;

    hcnse_msleep(HCNSE_LOG_INIT_DELAY);

    return HCNSE_OK;

failed:
    if (log) {
        hcnse_free(log);
    }
    if (file) {
        hcnse_file_fini(file);
    }

    if (messages != MAP_FAILED) {
        munmap(messages, sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE);
    }

    return err;
}

#else
hcnse_err_t
hcnse_log_init(hcnse_log_t **in_log, const char *fname, uint8_t level, size_t size)
{
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    void *ptr;

    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    hcnse_log_message_t *messages = NULL;
    size_t mem_size = 0;
    ssize_t file_size;
    hcnse_thread_t *tid = NULL;

    hcnse_err_t err;

    log = hcnse_malloc(sizeof(hcnse_log_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }
    memset(log, 0, sizeof(hcnse_log_t));

    tid = hcnse_malloc(sizeof(hcnse_thread_t));
    if (!tid) {
        err = hcnse_get_errno();
        goto failed;
    }
    memset(tid, 0, sizeof(hcnse_thread_t));

    file = hcnse_malloc(sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, fname, HCNSE_FILE_WRONLY,
                                    HCNSE_FILE_CREATE_OR_OPEN,
                                            HCNSE_FILE_OWNER_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }
    if (((size_t) file_size) >= size) {
        err = HCNSE_ERR_LOG_BIG;
        goto failed;
    }
    file->offset = file_size;

    mem_size += sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE;
    mem_size += sizeof(hcnse_mutex_t) * 2;
    mem_size += sizeof(hcnse_semaphore_t) * 2;

    messages = hcnse_malloc(mem_size);
    if (messages == NULL) {
        err = hcnse_get_errno();
        goto failed;
    }

    ptr = messages;
    ptr += sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE;

    mutex_deposit = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    mutex_fetch = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    sem_empty = (hcnse_semaphore_t *) ptr;
    ptr += sizeof(hcnse_semaphore_t);

    sem_full = (hcnse_semaphore_t *) ptr;

    err = hcnse_semaphore_init(sem_empty, HCNSE_LOG_BUF_SIZE,
                            HCNSE_LOG_BUF_SIZE, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_semaphore_init(sem_full, 0,
                            HCNSE_LOG_BUF_SIZE, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_mutex_init(mutex_deposit, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_mutex_init(mutex_fetch, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }

    /* XXX: Init all log struct fields before run worker */
    log->file = file;
    log->level = level;
    log->size = size;
    log->messages = messages;
    log->mutex_deposit = mutex_deposit;
    log->mutex_fetch = mutex_fetch;
    log->sem_empty = sem_empty;
    log->sem_full = sem_full;
    log->tid = tid;

    err = hcnse_thread_create(tid,
        HCNSE_THREAD_SCOPE_SYSTEM|HCNSE_THREAD_CREATE_JOINABLE,
        0, HCNSE_THREAD_PRIORITY_NORMAL, hcnse_log_worker, (void *) log);
    if (err != HCNSE_OK) {
        goto failed;
    }

    *in_log = log;

    hcnse_msleep(HCNSE_LOG_INIT_DELAY);

    return HCNSE_OK;

failed:
    if (log) {
        hcnse_free(log);
    }
    if (tid) {
        hcnse_free(tid);
    }
    if (file) {
        hcnse_file_fini(file);
    }

    if (messages) {
        hcnse_free(messages);
    }

    return err;
}
#endif

void
hcnse_log_msg(uint8_t level, hcnse_log_t *log, const char *fmt, ...)
{
    hcnse_log_message_t *messages = log->messages;
    hcnse_log_message_t *msg;
    va_list args;

    if (level > (log->level)) {
        return;
    }

    hcnse_semaphore_wait(log->sem_empty);
    hcnse_mutex_lock(log->mutex_deposit);

    msg = &(messages[log->rear]);
    log->rear = ((log->rear) + 1) % HCNSE_LOG_BUF_SIZE;

    va_start(args, fmt);
    vsnprintf(msg->str, HCNSE_LOG_MSG_SIZE, fmt, args);
    va_end(args);

    hcnse_timestr(msg->time, HCNSE_TIMESTRLEN, time(NULL));
    msg->level = level;

    hcnse_mutex_unlock(log->mutex_deposit);
    hcnse_semaphore_post(log->sem_full);
}

void
hcnse_log_error(uint8_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...)
{
    char *buf;
    size_t len;
    hcnse_log_message_t *messages = log->messages;
    hcnse_log_message_t *msg;
    va_list args;

    if (level > (log->level)) {
        return;
    }

    hcnse_semaphore_wait(log->sem_empty);
    hcnse_mutex_lock(log->mutex_deposit);

    msg = &(messages[log->rear]);
    log->rear = ((log->rear) + 1) % HCNSE_LOG_BUF_SIZE;

    va_start(args, fmt);
    len = vsnprintf(msg->str, HCNSE_LOG_MSG_SIZE, fmt, args);
    va_end(args);

    if (err != HCNSE_OK) {
        if (HCNSE_LOG_MSG_SIZE > len) {
            buf = (msg->str) + len;
            snprintf(buf, HCNSE_LOG_MSG_SIZE, " (%d: %s)",
                err, hcnse_strerror(err));
        }
    }

    hcnse_timestr(msg->time, HCNSE_TIMESTRLEN, time(NULL));
    msg->level = level;

    hcnse_mutex_unlock(log->mutex_deposit);
    hcnse_semaphore_post(log->sem_full);
}

void
hcnse_log_fini(hcnse_log_t *log)
{
    hcnse_log_message_t *messages = log->messages;

    hcnse_file_fini(log->file);
    hcnse_mutex_fini(log->mutex_deposit);
    hcnse_mutex_fini(log->mutex_fetch);
    hcnse_semaphore_fini(log->sem_full);
    hcnse_semaphore_fini(log->sem_empty);

#if (HCNSE_UNIX && HCNSE_HAVE_MMAP)
    kill(log->pid, SIGKILL);
    munmap(messages, sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE);
#else
    hcnse_thread_cancel(log->tid);
    hcnse_thread_destroy(log->tid);
    hcnse_free(messages);
    hcnse_free(log->tid);
#endif
    hcnse_free(log);
}
