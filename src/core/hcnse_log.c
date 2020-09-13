#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_MAX_LOG_SLOTS            32
#define HCNSE_LOG_STR_SIZE             1000
#define HCNSE_LOG_TIMESTR_SIZE         24
#define HCNSE_LOG_TOTAL_STR_SIZE  (HCNSE_LOG_STR_SIZE + HCNSE_LOG_TIMESTR_SIZE)

#define HCNSE_LOG_INIT_DELAY           500
#define HCNSE_LOG_WORKER_DELAY         1000

typedef void (*hcnse_log_handler_t) (hcnse_log_t *log, hcnse_uint_t level,
    char *buf, size_t len);

typedef struct {
    hcnse_uint_t level;
    char time[HCNSE_LOG_TIMESTR_SIZE];
    char str[HCNSE_LOG_STR_SIZE];
} hcnse_log_message_t;

struct hcnse_log_s {
    hcnse_uint_t level;
    hcnse_file_t *file;
    size_t size; /* 0 - unlimited */
    hcnse_log_handler_t handler;
};

struct hcnse_logger_s {
    hcnse_pool_t *pool;
    hcnse_list_t *logs; /* hcnse_log_t */
    hcnse_log_message_t *messages;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    pid_t pid;
#else
    hcnse_thread_t *tid;
#endif

    hcnse_flag_t running;

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
    "warn",
    "info",
    "debug",
    NULL
};


static void
hcnse_write_to_log(hcnse_log_t *log, hcnse_uint_t level,
    char *buf, size_t len)
{
    if (level > (log->level)) {
        return;
    }

    if (log->size) {
        if ((log->file->offset + len) > (log->size)) {
            log->file->offset = 0;
        }
    }

    hcnse_write_fd(log->file->fd, buf, len);
}

static hcnse_thread_value_t
hcnse_logger_worker(void *arg)
{
    char buf[HCNSE_LOG_TOTAL_STR_SIZE];
    hcnse_logger_t *logger;  
    hcnse_log_message_t *messages, *message;
    size_t len;
    hcnse_list_node_t *iter;
    hcnse_log_t *log;

    logger = (hcnse_logger_t *) arg;
    messages = logger->messages;

    hcnse_msleep(HCNSE_LOG_WORKER_DELAY);

    while (1) {
        hcnse_semaphore_wait(logger->sem_full);
        hcnse_mutex_lock(logger->mutex_fetch);

        message = &(messages[logger->front]);
        logger->front = ((logger->front) + 1) % HCNSE_MAX_LOG_SLOTS;

        len = hcnse_snprintf(buf, HCNSE_LOG_TOTAL_STR_SIZE,
            "%s [%s] %s" HCNSE_EOL_STR, 
            message->time, hcnse_log_prio[message->level], message->str);

        iter = hcnse_list_first(logger->logs);
        for ( ; iter; iter = hcnse_list_next(iter)) {
            log = hcnse_list_data(iter);
            log->handler(log, message->level, buf, len);
        }

        hcnse_mutex_unlock(logger->mutex_fetch);
        hcnse_semaphore_post(logger->sem_empty);
    }
    return 0;
}


hcnse_err_t
hcnse_logger_create1(hcnse_logger_t **out_logger)
{
    hcnse_mutex_t *mutex_deposit;
    hcnse_mutex_t *mutex_fetch;
    hcnse_semaphore_t *sem_empty;
    hcnse_semaphore_t *sem_full;
    uint8_t *mem;

    hcnse_pool_t *pool;
    hcnse_logger_t *logger;
    size_t mem_size;

    hcnse_list_t *logs;
    hcnse_err_t err;


#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    hcnse_log_message_t *messages;
    messages = MAP_FAILED;
#else
    hcnse_log_message_t *messages;
    messages = NULL;
#endif

    logger = NULL;
    mem_size = 0;

    pool = hcnse_pool_create(0, NULL);
    if (!pool) {
        err = hcnse_get_errno();
        goto failed;
    }

    logger = hcnse_pcalloc(pool, sizeof(hcnse_logger_t));
    if (!logger) {
        err = hcnse_get_errno();
        goto failed;
    }

    logs = hcnse_list_create(pool);
    if (!logs) {
        err = hcnse_get_errno();
        goto failed;
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

    mem = (uint8_t *) messages;
    mem += sizeof(hcnse_log_message_t) * HCNSE_MAX_LOG_SLOTS;

    mutex_deposit = (hcnse_mutex_t *) mem;
    mem += sizeof(hcnse_mutex_t);

    mutex_fetch = (hcnse_mutex_t *) mem;
    mem += sizeof(hcnse_mutex_t);

    sem_empty = (hcnse_semaphore_t *) mem;
    mem += sizeof(hcnse_semaphore_t);

    sem_full = (hcnse_semaphore_t *) mem;

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

    logger->pool = pool;
    logger->logs = logs;
    logger->messages = messages;
    logger->running = 0;
    logger->mutex_deposit = mutex_deposit;
    logger->mutex_fetch = mutex_fetch;
    logger->sem_empty = sem_empty;
    logger->sem_full = sem_full;

    *out_logger = logger;

    return HCNSE_OK;

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
hcnse_logger_create(void)
{
    hcnse_logger_t *logger;

    if (hcnse_logger_create1(&logger) != HCNSE_OK) {
        return NULL;
    }
    return logger;
}


hcnse_err_t
hcnse_logger_add_log_fd(hcnse_logger_t *logger, hcnse_uint_t level,
    hcnse_fd_t fd)
{
    hcnse_log_t *log;
    hcnse_file_t *file;
    hcnse_err_t err;

    if (logger->running) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
            "Failed to add logs to a running logger process");
        err = HCNSE_FAILED;
        goto failed;
    }

    log = hcnse_pcalloc(logger->pool, sizeof(hcnse_log_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }

    file = hcnse_palloc(logger->pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    file->fd = fd;

    log->level = level;
    log->file = file;
    log->size = 0;
    log->handler = hcnse_write_to_log;

    hcnse_list_push_back(logger->logs, log);

    return HCNSE_OK;

failed:

    return err;
}


hcnse_err_t
hcnse_logger_add_log_file(hcnse_logger_t *logger, hcnse_uint_t level,
    const char *fname, size_t size)
{
    hcnse_log_t *log;
    hcnse_file_t *file;
    ssize_t file_size;
    hcnse_err_t err;

    if (logger->running) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
            "Failed to add logs to a running logger process");
        err = HCNSE_FAILED;
        goto failed;
    }

    log = hcnse_pcalloc(logger->pool, sizeof(hcnse_log_t));
    if (!log) {
        err = hcnse_get_errno();
        goto failed;
    }

    file = hcnse_palloc(logger->pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_open(file, fname, HCNSE_FILE_WRONLY,
        HCNSE_FILE_CREATE_OR_APPEND, HCNSE_FILE_OWNER_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(logger->pool, file, hcnse_file_close);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    if (size) { /* Not unlimited */
        if (((size_t) file_size) >= size) {
            file->offset = 0;
        }
        else {
            file->offset = file_size;
        }
    }

    log->level = level;
    log->file = file;
    log->size = size;
    log->handler = hcnse_write_to_log;

    hcnse_list_push_back(logger->logs, log);

    return HCNSE_OK;

failed:

    return err;
}

hcnse_err_t
hcnse_logger_start(hcnse_logger_t *logger)
{
    hcnse_err_t err;

    logger->running = 1;

#if (HCNSE_POSIX && HCNSE_HAVE_MMAP)
    logger->pid = fork();
    if (logger->pid == -1) {
        return hcnse_get_errno();
    }
    if (!pid) {
        hcnse_logger_worker(logger);
    }

#else
    logger->tid = hcnse_palloc(logger->pool, sizeof(hcnse_thread_t));
    if (!logger->tid) {
        return hcnse_get_errno();
    }

    err = hcnse_thread_init(logger->tid,
        HCNSE_THREAD_SCOPE_SYSTEM|HCNSE_THREAD_CREATE_DETACHED, 0,
            HCNSE_THREAD_PRIORITY_NORMAL, hcnse_logger_worker, (void *) logger);

    if (err != HCNSE_OK) {
        return err;
    }
    hcnse_pool_cleanup_add(logger->pool, logger->tid, hcnse_thread_fini);

#endif

    hcnse_msleep(HCNSE_LOG_INIT_DELAY);


    return HCNSE_OK;
}

void
hcnse_log_error(hcnse_uint_t level, hcnse_logger_t *logger, hcnse_err_t err,
    const char *fmt, ...)
{
    char errstr[HCNSE_ERRNO_STR_SIZE];
    char *buf;
    hcnse_log_message_t *messages, *message;
    hcnse_tm_t tm;
    size_t len;
    va_list args;

    /* Set time before slot waiting */
    hcnse_localtime(time(NULL), &tm);

    /* Wait to available log slot */
    hcnse_semaphore_wait(logger->sem_empty);
    hcnse_mutex_lock(logger->mutex_deposit);

    messages = logger->messages;
    message = &(messages[logger->rear]);
    logger->rear = ((logger->rear) + 1) % HCNSE_MAX_LOG_SLOTS;

    va_start(args, fmt);
    len = hcnse_vsnprintf(message->str, HCNSE_LOG_STR_SIZE, fmt, args);
    va_end(args);

    /* If error code is OK, don't log description of error */
    if (err != HCNSE_OK) {
        if (len < HCNSE_LOG_STR_SIZE ) {
            buf = (message->str) + len;
            hcnse_snprintf(buf, HCNSE_LOG_STR_SIZE - len, " (%d: %s)",
                err, hcnse_strerror(err, errstr, HCNSE_ERRNO_STR_SIZE));
        }
    }

    hcnse_snprintf(message->time, HCNSE_LOG_TIMESTR_SIZE,
        "[%02d.%02d.%02d] [%02d:%02d:%02d]",
        tm.hcnse_tm_mday, tm.hcnse_tm_mon, tm.hcnse_tm_year,
        tm.hcnse_tm_hour, tm.hcnse_tm_min, tm.hcnse_tm_sec);

    message->level = level;

    /* Post to available log slot */
    hcnse_mutex_unlock(logger->mutex_deposit);
    hcnse_semaphore_post(logger->sem_full);
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
hcnse_logger_destroy(hcnse_logger_t *log)
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
