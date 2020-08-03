#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_MAX_LOG_SLOTS            32
#define HCNSE_LOG_STR_SIZE             1000
#define HCNSE_LOG_TIMESTR_SIZE         24
#define HCNSE_LOG_TOTAL_STR_SIZE  (HCNSE_LOG_STR_SIZE + HCNSE_LOG_TIMESTR_SIZE)

#define HCNSE_LOG_INIT_DELAY           500
#define HCNSE_LOG_WORKER_DELAY         1000

typedef void (*hcnse_log_handler_t) (hcnse_logger_t *log, hcnse_uint_t level,
    char *buf, size_t len);

/*
 * TODO:
 * A logger instance can be a chain of loggers, linked to each other
 * with the next field. In this case, each message is written to all loggers
 * in the chain
 * 
 * fix conf to:
 * # log <file> <level> <size/nolimit>
 * 
 */

typedef struct {
    hcnse_uint_t level;
    hcnse_file_t *file;
    size_t max_size;
    hcnse_uint_t rewrite;
    hcnse_log_handler_t handler;
} hcnse_log_output_t;

typedef struct {
    hcnse_uint_t level;
    char time[HCNSE_LOG_TIMESTR_SIZE];
    char str[HCNSE_LOG_STR_SIZE];
} hcnse_log_message_t;

struct hcnse_logger_s {
    hcnse_pool_t *pool;
    hcnse_list_t *outputs;


    hcnse_uint_t level; /* remove */
    hcnse_file_t *file;  /* remove */
    size_t file_max_size;  /* remove */
    hcnse_uint_t file_rewrite;  /* remove */

    hcnse_log_message_t *messages;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    pid_t pid;
#else
    hcnse_thread_t *tid;
#endif

    hcnse_uint_t front;
    hcnse_uint_t rear;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
};


hcnse_logger_t *hcnse_logger_global;

const char *hcnse_log_prio[] = {
    "emerg",
    "error",
    "warning",
    "info",
    "debug"
};


static hcnse_thread_value_t
hcnse_log_worker(void *arg)
{
    char buf[HCNSE_LOG_TOTAL_STR_SIZE];
    hcnse_logger_t *log;
    hcnse_log_message_t *messages, *msg;
    size_t len;


    log = (hcnse_logger_t *) arg;
    messages = log->messages;

    hcnse_msleep(HCNSE_LOG_WORKER_DELAY);

    while (1) {
        hcnse_semaphore_wait(log->sem_full);
        hcnse_mutex_lock(log->mutex_fetch);

        msg = &(messages[log->front]);
        log->front = ((log->front) + 1) % HCNSE_MAX_LOG_SLOTS;

        len = hcnse_snprintf(buf, HCNSE_LOG_TOTAL_STR_SIZE,
            "%s [%s] %s" HCNSE_EOL_STR, 
            msg->time, hcnse_log_prio[msg->level], msg->str);

        /* TODO: Improve logs rotation */
        if (log->file_max_size) {
            if ((log->file->offset + len) > (log->file_max_size)) {
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

#if 0
hcnse_err_t
hcnse_log_create1(hcnse_logger_t **in_log, hcnse_conf_t *conf)
{

    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    void *ptr;

    hcnse_pool_t *pool;
    hcnse_logger_t *log = NULL;
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

    hcnse_list_node_t *iter;
    hcnse_list_t *outputs;
    hcnse_err_t err;

    pool = hcnse_pool_create(0, NULL);
    if (!pool) {
        err = hcnse_get_errno();
        goto failed;
    }

    log = hcnse_pcalloc(pool, sizeof(hcnse_logger_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }

    outputs = hcnse_list_create(pool);
    if (!outputs) {
        err = hcnse_get_errno();
        goto failed;
    }

    iter = hcnse_list_first(conf->log_outputs);
    for ( ; iter; iter = iter->next) {
        printf("%s\n", (char *) iter->data);
        iter = iter->next;
        printf("%s\n", (char *) iter->data);
        iter = iter->next;
        printf("%s\n", (char *) iter->data);
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

    mem_size += sizeof(hcnse_log_message_t) * HCNSE_MAX_LOG_SLOTS;
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
    ptr += sizeof(hcnse_log_message_t) * HCNSE_MAX_LOG_SLOTS;

    mutex_deposit = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    mutex_fetch = (hcnse_mutex_t *) ptr;
    ptr += sizeof(hcnse_mutex_t);

    sem_empty = (hcnse_semaphore_t *) ptr;
    ptr += sizeof(hcnse_semaphore_t);

    sem_full = (hcnse_semaphore_t *) ptr;

    err = hcnse_semaphore_init(sem_empty, HCNSE_MAX_LOG_SLOTS,
                            HCNSE_MAX_LOG_SLOTS, HCNSE_SEMAPHORE_SHARED);
    if (err != HCNSE_OK) {
        goto failed;
    }
    hcnse_pool_cleanup_add(pool, sem_empty, hcnse_semaphore_fini);

    err = hcnse_semaphore_init(sem_full, 0,
                            HCNSE_MAX_LOG_SLOTS, HCNSE_SEMAPHORE_SHARED);
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
    log->file_max_size = conf->log_size;
    log->file_rewrite = conf->log_rewrite;
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
#endif

#if 0
failed:
    if (pool) {
        hcnse_pool_destroy(pool);
    }

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    if (messages != MAP_FAILED) {
        munmap(messages, sizeof(hcnse_log_message_t) * HCNSE_MAX_LOG_SLOTS);
    }
#endif
    return err;

}

hcnse_logger_t *
hcnse_log_create(hcnse_conf_t *conf)
{
    hcnse_logger_t *log;

    if (hcnse_log_create1(&log, conf) != HCNSE_OK) {
        return NULL;
    }
    return log;
}

#endif

void
hcnse_log_error(hcnse_uint_t level, hcnse_logger_t *log, hcnse_err_t err,
    const char *fmt, ...)
{
    char errstr[HCNSE_ERRNO_STR_SIZE];
    char *buf;
    hcnse_log_message_t *messages, *msg;
    hcnse_tm_t tm;
    size_t len;
    va_list args;


    if (level > (log->level)) {
        return;
    }

    /* Set time before slot waiting */
    hcnse_localtime(time(NULL), &tm);

    /* Wait to available log slot */
    hcnse_semaphore_wait(log->sem_empty);
    hcnse_mutex_lock(log->mutex_deposit);

    messages = log->messages;
    msg = &(messages[log->rear]);
    log->rear = ((log->rear) + 1) % HCNSE_MAX_LOG_SLOTS;

    va_start(args, fmt);
    len = hcnse_vsnprintf(msg->str, HCNSE_LOG_STR_SIZE, fmt, args);
    va_end(args);

    /* If error code is OK, don't log description of error */
    if (err != HCNSE_OK) {
        if (len < HCNSE_LOG_STR_SIZE ) {
            buf = (msg->str) + len;
            hcnse_snprintf(buf, HCNSE_LOG_STR_SIZE - len, " (%d: %s)",
                err, hcnse_strerror(err, errstr, HCNSE_ERRNO_STR_SIZE));
        }
    }

    hcnse_snprintf(msg->time, HCNSE_LOG_TIMESTR_SIZE,
        "[%02d.%02d.%02d] [%02d:%02d:%02d]",
        tm.hcnse_tm_mday, tm.hcnse_tm_mon, tm.hcnse_tm_year,
        tm.hcnse_tm_hour, tm.hcnse_tm_min, tm.hcnse_tm_sec);

    msg->level = level;

    /* Post to available log slot */
    hcnse_mutex_unlock(log->mutex_deposit);
    hcnse_semaphore_post(log->sem_full);
}

void
hcnse_log_console(hcnse_fd_t fd, hcnse_err_t err, const char *fmt, ...)
{
    char logstr[HCNSE_LOG_STR_SIZE];
    char errstr[HCNSE_ERRNO_STR_SIZE];
    char *pos;
    size_t len, n;
    va_list args;


    va_start(args, fmt);
    len = hcnse_vsnprintf(logstr, HCNSE_LOG_STR_SIZE, fmt, args);
    va_end(args);

    if (len < HCNSE_LOG_STR_SIZE) {

        pos = logstr + len;

        if (err != HCNSE_OK) {
            n = hcnse_snprintf(pos, HCNSE_LOG_STR_SIZE - len, " (%d: %s)\n",
                err, hcnse_strerror(err, errstr, HCNSE_ERRNO_STR_SIZE));
        }
        else {
            n = hcnse_snprintf(pos, HCNSE_LOG_STR_SIZE - len, "\n");
        }

        len += n;
    }
    else {
        logstr[HCNSE_LOG_STR_SIZE - 1] = '\n';
    }

    hcnse_write_fd(fd, logstr, len);
}

void
hcnse_log_destroy(hcnse_logger_t *log)
{
#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    hcnse_log_message_t *temp;
    temp = log->messages;
    kill(log->pid, SIGKILL);
    hcnse_pool_destroy(log->pool);
    munmap(temp, sizeof(hcnse_log_message_t) * HCNSE_MAX_LOG_SLOTS);
#else
    hcnse_thread_cancel(log->tid);
    hcnse_msleep(100); /* Wait thread terminate */
    hcnse_pool_destroy(log->pool);
#endif
}
