#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LOG_MSG_SIZE             500
#define HCNSE_LOG_BUF_SIZE             32

#define HCNSE_LOG_INIT_DELAY           500
#define HCNSE_LOG_WORKER_DELAY         1000


typedef struct {
    uint8_t level;
    char time[HCNSE_TIMESTRLEN];
    char str[HCNSE_LOG_MSG_SIZE];
} hcnse_log_message_t;

struct hcnse_log_s {
    hcnse_pool_t *pool;
    hcnse_file_t *file;
    uint8_t level;
    size_t size;
    bool rewrite;
    hcnse_log_message_t *messages;
#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
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

hcnse_log_t *hcnse_log_global;


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

        len = hcnse_snprintf(buf, maxlen, "%s [%s] %s" HCNSE_PORTABLE_LF, 
                                msg->time, prio[msg->level], msg->str);

        /* TODO: Improve logs rotation */
        if (log->size) {
            if ((log->file->offset + len) > (log->size)) {
                log->file->offset = 0;
            }
        }

        if (hcnse_write_fd(log->file->fd, buf, len) == -1) {
            /* XXX: What we need to do in this statement? */
        }

#if (HCNSE_DEBUG && HCNSE_NO_DAEMON)
        hcnse_write_fd(HCNSE_STDOUT, buf, len);
#endif

        hcnse_mutex_unlock(log->mutex_fetch);
        hcnse_semaphore_post(log->sem_empty);
    }
    return 0;
}

hcnse_err_t
hcnse_log_create1(hcnse_log_t **in_log, hcnse_conf_t *conf)
{
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    void *ptr;

    hcnse_pool_t *pool;
    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    size_t mem_size = 0;
    ssize_t file_size;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    hcnse_log_message_t *messages = MAP_FAILED;
    pid_t pid;
#else
    hcnse_log_message_t *messages = NULL;
    hcnse_thread_t *tid = NULL;
#endif

    hcnse_err_t err;


    pool = hcnse_pool_create(NULL);
    if (!pool) {
        err = hcnse_get_errno();
        goto failed;
    }

    log = hcnse_pcalloc(pool, sizeof(hcnse_log_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }

    file = hcnse_palloc(pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, conf->log_fname, HCNSE_FILE_WRONLY,
                                    HCNSE_FILE_CREATE_OR_OPEN,
                                            HCNSE_FILE_OWNER_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_fini);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }
    if (((size_t) file_size) >= conf->log_size || conf->log_rewrite) {
        file->offset = 0;
    }
    else {
        file->offset = file_size;
    }

    mem_size += sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE;
    mem_size += sizeof(hcnse_mutex_t) * 2;
    mem_size += sizeof(hcnse_semaphore_t) * 2;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    messages = mmap(NULL, mem_size, PROT_READ|PROT_WRITE,
                                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (messages == MAP_FAILED) {
        err = hcnse_get_errno();
        goto failed;
    }
#else
    messages = hcnse_palloc(pool, mem_size);
    if (messages == NULL) {
        err = hcnse_get_errno();
        goto failed;
    }
#endif

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
    hcnse_pool_cleanup_add(pool, sem_empty, hcnse_semaphore_fini);

    err = hcnse_semaphore_init(sem_full, 0,
                            HCNSE_LOG_BUF_SIZE, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }
    hcnse_pool_cleanup_add(pool, sem_full, hcnse_semaphore_fini);

    err = hcnse_mutex_init(mutex_deposit, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }
    hcnse_pool_cleanup_add(pool, mutex_deposit, hcnse_mutex_fini);

    err = hcnse_mutex_init(mutex_fetch, HCNSE_MUTEX_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }
    hcnse_pool_cleanup_add(pool, mutex_fetch, hcnse_mutex_fini);


    /* XXX: Init all log struct fields before run worker */
    log->pool = pool;
    log->file = file;
    log->level = conf->log_level;
    log->size = conf->log_size;
    log->rewrite = conf->log_rewrite;
    log->messages = messages;
    log->mutex_deposit = mutex_deposit;
    log->mutex_fetch = mutex_fetch;
    log->sem_empty = sem_empty;
    log->sem_full = sem_full;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
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
#else
    tid = hcnse_palloc(pool, sizeof(hcnse_thread_t));
    if (!tid) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_thread_init(tid,
        HCNSE_THREAD_SCOPE_SYSTEM|HCNSE_THREAD_CREATE_DETACHED,
        0, HCNSE_THREAD_PRIORITY_NORMAL, hcnse_log_worker, (void *) log);
    if (err != HCNSE_OK) {
        goto failed;
    }
    hcnse_pool_cleanup_add(pool, tid, hcnse_thread_fini);

    log->tid = tid;
#endif

    hcnse_msleep(HCNSE_LOG_INIT_DELAY);

    *in_log = log;
    return HCNSE_OK;

failed:
    if (pool) {
        hcnse_pool_destroy(pool);
    }

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    if (messages != MAP_FAILED) {
        munmap(messages, sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE);
    }
#endif
    return err;
}

hcnse_log_t *
hcnse_log_create(hcnse_conf_t *conf)
{
    hcnse_log_t *log;

    if (hcnse_log_create1(&log, conf) != HCNSE_OK) {
        return NULL;
    }
    return log;
}

void
hcnse_log_error(uint8_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...)
{
    hcnse_log_message_t *messages;
    hcnse_log_message_t *msg;
    va_list args;
    char *buf;
    size_t len;

    messages = log->messages;

    if (level > (log->level)) {
        return;
    }

    hcnse_semaphore_wait(log->sem_empty);
    hcnse_mutex_lock(log->mutex_deposit);

    msg = &(messages[log->rear]);
    log->rear = ((log->rear) + 1) % HCNSE_LOG_BUF_SIZE;

    va_start(args, fmt);
    len = hcnse_vsnprintf(msg->str, HCNSE_LOG_MSG_SIZE, fmt, args);
    va_end(args);

    if (err != HCNSE_OK) {
        if (HCNSE_LOG_MSG_SIZE > len) {
            buf = (msg->str) + len;
            hcnse_snprintf(buf, HCNSE_LOG_MSG_SIZE - len, " (%d: %s)",
                err, hcnse_strerror(err));
        }
    }

    hcnse_timestr(msg->time, HCNSE_TIMESTRLEN, time(NULL));
    msg->level = level;

    hcnse_mutex_unlock(log->mutex_deposit);
    hcnse_semaphore_post(log->sem_full);
}

void
hcnse_log_console(hcnse_fd_t fd, hcnse_err_t err, const char *fmt, ...)
{
    va_list args;
    size_t len1 = 0;
    size_t len2 = 0;
    static char buf1[HCNSE_LOG_MSG_SIZE];
    char *buf2;

    va_start(args, fmt);
    len1 = hcnse_vsnprintf(buf1, HCNSE_LOG_MSG_SIZE, fmt, args);
    va_end(args);

    if (HCNSE_LOG_MSG_SIZE > len1) {
        buf2 = buf1 + len1;
        if (err != HCNSE_OK) {
            len2 = hcnse_snprintf(buf2, HCNSE_LOG_MSG_SIZE - len1,
                " (%d: %s)\n", err, hcnse_strerror(err));
        }
        else {
            len2 = hcnse_snprintf(buf2, HCNSE_LOG_MSG_SIZE - len1, "\n");
        }
    }
    hcnse_write_fd(fd, buf1, len1 + len2);
}

void
hcnse_log_destroy(hcnse_log_t *log)
{
#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    hcnse_log_message_t *temp;
    temp = log->messages;
    kill(log->pid, SIGKILL);
    hcnse_pool_destroy(log->pool);
    munmap(temp, sizeof(hcnse_log_message_t) * HCNSE_LOG_BUF_SIZE);
#else
    hcnse_thread_cancel(log->tid);
    hcnse_msleep(100); /* Wait thread terminate */
    hcnse_pool_destroy(log->pool);
#endif
}
