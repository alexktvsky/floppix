#include "fpx.core.log.h"
#include "fpx.system.pool.h"
#include "fpx.system.thread.h"
#include "fpx.system.sync.mutex.h"
#include "fpx.system.sync.semaphore.h"
#include "fpx.system.time.h"
#include "fpx.algo.list.h"
#include "fpx.util.string.h"

#define FPX_MAX_LOG_SLOTS      32
#define FPX_LOG_STR_SIZE       1000
#define FPX_LOG_TIMESTR_SIZE   24
#define FPX_LOG_TOTAL_STR_SIZE (FPX_LOG_STR_SIZE + FPX_LOG_TIMESTR_SIZE)

#define FPX_LOG_INIT_DELAY     500
#define FPX_LOG_WORKER_DELAY   1000

typedef void (*fpx_log_handler_t)(fpx_log_t *log, fpx_uint_t level, char *buf,
    fpx_size_t len);

typedef struct {
    fpx_uint_t level;
    char time[FPX_LOG_TIMESTR_SIZE];
    char str[FPX_LOG_STR_SIZE];
} fpx_log_message_t;

struct fpx_log_s {
    fpx_uint_t level;
    fpx_file_t *file;
    fpx_size_t size; /* 0 - unlimited */
    fpx_log_handler_t handler;
};

struct fpx_logger_s {
    fpx_pool_t *pool;
    fpx_list_t *logs; /* fpx_log_t */
    fpx_log_message_t *messages;

#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    pid_t pid;
#else
    fpx_thread_t *tid;
#endif

    fpx_bool_t running;

    fpx_uint_t front;
    fpx_uint_t rear;
    fpx_semaphore_t *sem_empty;
    fpx_semaphore_t *sem_full;
    fpx_mutex_t *mutex_deposit;
    fpx_mutex_t *mutex_fetch;
};

fpx_logger_t *fpx_logger_global;

const char *fpx_log_prio[] = {
    "emerg",
    "error",
    "warn",
    "info",
    "debug",
    NULL,
};

static void
fpx_write_to_log(fpx_log_t *log, fpx_uint_t level, char *buf, fpx_size_t len)
{
    if (level > (log->level)) {
        return;
    }

    if (log->size) {
        if ((log->file->offset + len) > (log->size)) {
            log->file->offset = 0;
        }
    }

    fpx_write_fd(log->file->fd, buf, len);

    /* TODO: What about high performance? */
    fpx_fsync(log->file->fd);
}

/*
 * Notes on log rotation:
 * The only way to replace text in a file, or add lines in the middle of
 * a file, is to rewrite the entire file from the point of the first
 * modification. You cannot "make space" in the middle of a file for new lines.
 */

static fpx_thread_value_t
fpx_logger_worker(void *arg)
{
    char buf[FPX_LOG_TOTAL_STR_SIZE];
    fpx_logger_t *logger;
    fpx_log_message_t *messages, *message;
    fpx_size_t len;
    fpx_list_node_t *node;
    fpx_log_t *log;

    logger = (fpx_logger_t *) arg;
    messages = logger->messages;

    fpx_msleep(FPX_LOG_WORKER_DELAY);

    /*
     * Fixme: Why process log can not get in time?
     */
#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    fpx_log_debug1(FPX_OK, "The log process has been started with pid %d",
        getpid());
#else
    fpx_log_debug1(FPX_OK,
        "The log thread has been started with tid " FPX_FMT_TID_T,
        fpx_thread_current_tid());
#endif

    while (1) {
        fpx_semaphore_wait(logger->sem_full);
        fpx_mutex_lock(logger->mutex_fetch);

        message = &(messages[logger->front]);
        logger->front = ((logger->front) + 1) % FPX_MAX_LOG_SLOTS;

        len =
            fpx_snprintf(buf, FPX_LOG_TOTAL_STR_SIZE, "%s [%s] %s" FPX_EOL_STR,
                message->time, fpx_log_prio[message->level], message->str);

        node = logger->logs->head;
        for (; node; node = node->next) {
            log = (fpx_log_t *) node->data;
            log->handler(log, message->level, buf, len);
        }

        fpx_mutex_unlock(logger->mutex_fetch);
        fpx_semaphore_post(logger->sem_empty);
    }
    return 0;
}

fpx_err_t
fpx_logger_init(fpx_logger_t **out_logger)
{
    fpx_mutex_t *mutex_deposit;
    fpx_mutex_t *mutex_fetch;
    fpx_semaphore_t *sem_empty;
    fpx_semaphore_t *sem_full;
    uint8_t *mem;

    fpx_pool_t *pool;
    fpx_logger_t *logger;
    fpx_size_t mem_size;

    fpx_list_t *logs;
    fpx_err_t err;

#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    fpx_log_message_t *messages;
    messages = MAP_FAILED;
#else
    fpx_log_message_t *messages;
    messages = NULL;
#endif

    logger = NULL;
    mem_size = 0;

    if ((err = fpx_pool_create(&pool, 0, NULL)) != FPX_OK) {
        goto failed;
    }

    logger = fpx_pcalloc(pool, sizeof(fpx_logger_t));
    if (!logger) {
        err = fpx_get_errno();
        goto failed;
    }

    if ((err = fpx_list_init(&logs, pool)) != FPX_OK) {
        goto failed;
    }

    mem_size += sizeof(fpx_log_message_t) * FPX_MAX_LOG_SLOTS;
    mem_size += sizeof(fpx_mutex_t) * 2;
    mem_size += sizeof(fpx_semaphore_t) * 2;

#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    messages = mmap(NULL, mem_size, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (messages == MAP_FAILED) {
        err = fpx_get_errno();
        goto failed;
    }

    /* Fixme: add cleanup for mmap */

#else
    messages = fpx_palloc(pool, mem_size);
    if (messages == NULL) {
        err = fpx_get_errno();
        goto failed;
    }
#endif

    mem = (uint8_t *) messages;
    mem += sizeof(fpx_log_message_t) * FPX_MAX_LOG_SLOTS;

    mutex_deposit = (fpx_mutex_t *) mem;
    mem += sizeof(fpx_mutex_t);

    mutex_fetch = (fpx_mutex_t *) mem;
    mem += sizeof(fpx_mutex_t);

    sem_empty = (fpx_semaphore_t *) mem;
    mem += sizeof(fpx_semaphore_t);

    sem_full = (fpx_semaphore_t *) mem;

    err = fpx_semaphore_init(sem_empty, FPX_MAX_LOG_SLOTS, FPX_MAX_LOG_SLOTS,
        FPX_SEMAPHORE_SHARED);
    if (err != FPX_OK) {
        goto failed;
    }
    fpx_pool_cleanup_add(pool, sem_empty, fpx_semaphore_fini);

    err = fpx_semaphore_init(sem_full, 0, FPX_MAX_LOG_SLOTS,
        FPX_SEMAPHORE_SHARED);
    if (err != FPX_OK) {
        goto failed;
    }
    fpx_pool_cleanup_add(pool, sem_full, fpx_semaphore_fini);

    err = fpx_mutex_init(mutex_deposit, FPX_MUTEX_SHARED);
    if (err != FPX_OK) {
        goto failed;
    }
    fpx_pool_cleanup_add(pool, mutex_deposit, fpx_mutex_fini);

    err = fpx_mutex_init(mutex_fetch, FPX_MUTEX_SHARED);
    if (err != FPX_OK) {
        goto failed;
    }
    fpx_pool_cleanup_add(pool, mutex_fetch, fpx_mutex_fini);

    logger->pool = pool;
    logger->logs = logs;
    logger->messages = messages;
    logger->running = false;
    logger->mutex_deposit = mutex_deposit;
    logger->mutex_fetch = mutex_fetch;
    logger->sem_empty = sem_empty;
    logger->sem_full = sem_full;

    *out_logger = logger;

    return FPX_OK;

failed:
    if (pool) {
        fpx_pool_destroy(pool);
    }

#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    if (messages != MAP_FAILED) {
        munmap(messages, sizeof(fpx_log_message_t) * FPX_MAX_LOG_SLOTS);
    }
#endif
    return err;
}

fpx_err_t
fpx_logger_add_log_fd(fpx_logger_t *logger, fpx_uint_t level, fpx_fd_t fd)
{
    fpx_log_t *log;
    fpx_file_t *file;
    fpx_err_t err;

    if (logger->running) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Failed to add logs to a running logger process");
        err = FPX_FAILED;
        goto failed;
    }

    log = fpx_pcalloc(logger->pool, sizeof(fpx_log_t));
    if (!log) {
        err = fpx_get_errno();
        goto failed;
    }

    file = fpx_palloc(logger->pool, sizeof(fpx_file_t));
    if (!file) {
        err = fpx_get_errno();
        goto failed;
    }

    file->fd = fd;

    log->level = level;
    log->file = file;
    log->size = 0;
    log->handler = fpx_write_to_log;

    fpx_list_push_back(logger->logs, log);

    return FPX_OK;

failed:

    return err;
}

fpx_err_t
fpx_logger_add_log_file(fpx_logger_t *logger, fpx_uint_t level,
    const char *fname, fpx_size_t size)
{
    fpx_log_t *log;
    fpx_file_t *file;
    fpx_ssize_t file_size;
    fpx_err_t err;

    if (logger->running) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Failed to add logs to a running logger process");
        err = FPX_FAILED;
        goto failed;
    }

    log = fpx_pcalloc(logger->pool, sizeof(fpx_log_t));
    if (!log) {
        err = fpx_get_errno();
        goto failed;
    }

    file = fpx_palloc(logger->pool, sizeof(fpx_file_t));
    if (!file) {
        err = fpx_get_errno();
        goto failed;
    }

    err = fpx_file_open(file, fname, FPX_FILE_WRONLY, FPX_FILE_CREATE_OR_APPEND,
        FPX_FILE_OWNER_ACCESS);
    if (err != FPX_OK) {
        goto failed;
    }

    fpx_pool_cleanup_add(logger->pool, file, fpx_file_close);

    file_size = fpx_file_size(file);
    if (file_size == -1) {
        err = fpx_get_errno();
        goto failed;
    }

    if (size) { /* Not unlimited */
        if (((fpx_size_t) file_size) >= size) {
            file->offset = 0;
        }
        else {
            file->offset = file_size;
        }
    }

    log->level = level;
    log->file = file;
    log->size = size;
    log->handler = fpx_write_to_log;

    fpx_list_push_back(logger->logs, log);

    return FPX_OK;

failed:

    return err;
}

fpx_err_t
fpx_logger_start(fpx_logger_t *logger)
{
    fpx_err_t err;

    logger->running = true;

#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)

    (void) err;

    logger->pid = fork();
    if (logger->pid == -1) {
        return fpx_get_errno();
    }
    else if (logger->pid == 0) {
        fpx_logger_worker(logger);
    }

#else

    logger->tid = fpx_palloc(logger->pool, sizeof(fpx_thread_t));
    if (!logger->tid) {
        return fpx_get_errno();
    }

    err = fpx_thread_init(logger->tid,
        FPX_THREAD_SCOPE_SYSTEM | FPX_THREAD_CREATE_DETACHED, 0,
        FPX_THREAD_PRIORITY_NORMAL, fpx_logger_worker, (void *) logger);

    if (err != FPX_OK) {
        return err;
    }
    fpx_pool_cleanup_add(logger->pool, logger->tid, fpx_thread_fini);

#endif

    fpx_msleep(FPX_LOG_INIT_DELAY);

    return FPX_OK;
}

void
fpx_log_error(fpx_uint_t level, fpx_logger_t *logger, fpx_err_t err,
    const char *fmt, ...)
{
    char errstr[FPX_ERRNO_STR_SIZE];
    char *buf;
    fpx_log_message_t *messages, *message;
    fpx_tm_t tm;
    fpx_size_t len;
    va_list args;

    /* Set time before slot waiting */
    fpx_localtime(time(NULL), &tm);

    /* Wait to available log slot */
    fpx_semaphore_wait(logger->sem_empty);
    fpx_mutex_lock(logger->mutex_deposit);

    messages = logger->messages;
    message = &(messages[logger->rear]);
    logger->rear = ((logger->rear) + 1) % FPX_MAX_LOG_SLOTS;

    va_start(args, fmt);
    len = fpx_vsnprintf(message->str, FPX_LOG_STR_SIZE, fmt, args);
    va_end(args);

    /* If error code is OK, don't log description of error */
    if (err != FPX_OK) {
        if (len < FPX_LOG_STR_SIZE) {
            buf = (message->str) + len;
            fpx_snprintf(buf, FPX_LOG_STR_SIZE - len, " (%d: %s)", err,
                fpx_strerror(err, errstr, FPX_ERRNO_STR_SIZE));
        }
    }

    fpx_snprintf(message->time, FPX_LOG_TIMESTR_SIZE,
        "[%02d.%02d.%02d] [%02d:%02d:%02d]", tm.fpx_tm_mday, tm.fpx_tm_mon,
        tm.fpx_tm_year, tm.fpx_tm_hour, tm.fpx_tm_min, tm.fpx_tm_sec);

    message->level = level;

    /* Post to available log slot */
    fpx_mutex_unlock(logger->mutex_deposit);
    fpx_semaphore_post(logger->sem_full);
}

void
fpx_log_console(fpx_fd_t fd, fpx_err_t err, const char *fmt, ...)
{
    char logstr[FPX_LOG_STR_SIZE];
    char errstr[FPX_ERRNO_STR_SIZE];
    char *pos;
    fpx_size_t len, n;
    va_list args;

    va_start(args, fmt);
    len = fpx_vsnprintf(logstr, FPX_LOG_STR_SIZE, fmt, args);
    va_end(args);

    if (len < FPX_LOG_STR_SIZE) {

        pos = logstr + len;

        if (err != FPX_OK) {
            n = fpx_snprintf(pos, FPX_LOG_STR_SIZE - len, " (%d: %s)\n", err,
                fpx_strerror(err, errstr, FPX_ERRNO_STR_SIZE));
        }
        else {
            n = fpx_snprintf(pos, FPX_LOG_STR_SIZE - len, "\n");
        }

        len += n;
    }
    else {
        logstr[FPX_LOG_STR_SIZE - 1] = '\n';
    }

    fpx_write_fd(fd, logstr, len);
}

void
fpx_logger_destroy(fpx_logger_t *logger)
{
#if (FPX_POSIX && FPX_HAVE_MMAP && FPX_LOGGER_IS_PROC)
    fpx_log_message_t *messages;

    messages = logger->messages;
    if (running) {
        kill(logger->pid, SIGKILL);
        wait(NULL);
    }
    fpx_pool_destroy(logger->pool);

    /* Fixme: add cleanup for mmap to pool */
    munmap(messages, sizeof(fpx_log_message_t) * FPX_MAX_LOG_SLOTS);

#else
    if (logger->running) {
        fpx_thread_cancel(logger->tid);
        fpx_msleep(100); /* Wait thread terminate */
    }
    fpx_pool_destroy(logger->pool);
#endif
}
