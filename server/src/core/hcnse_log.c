#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LOG_BUF_SIZE             32
#define HCNSE_LOG_MSG_SIZE             300

#define HCNSE_LOG_INIT_DELAY           500
#define HCNSE_LOG_WORKER_DELAY         1000

#define HCNSE_LOG_SLEEP_COUNTER        3
#define HCNSE_LOG_SLEEP_TIME           5000


typedef struct {
    uint8_t level;
    char time[HCNSE_TIMESTRLEN];
    char str[HCNSE_LOG_MSG_SIZE];
} hcnse_log_message_t;

typedef struct {
    hcnse_mutex_t mutex_deposit;
    hcnse_mutex_t mutex_fetch;
    hcnse_log_message_t message;
} hcnse_shared_record_t;

struct hcnse_log_s {
    hcnse_file_t *file;
    uint8_t level;
    size_t size;
    hcnse_shared_record_t *buf;
    uint32_t front;
    uint32_t rear;
#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS) && (HCNSE_HAVE_MMAP)
    pid_t pid;
#else
    hcnse_thread_t *tid;
#endif
};

static const char *prio[] = {
    "emerg",
    "error",
    "warning",
    "info",
    "debug"
};

#if !(HCNSE_WINDOWS)
static void
hcnse_log_sigalrm_handler(int sig) {
    (void) sig;
    signal(SIGALRM, hcnse_log_sigalrm_handler);
}
#endif

static hcnse_log_message_t *
hcnse_log_try_write(hcnse_log_t *log)
{
    hcnse_shared_record_t *buf = log->buf;
    hcnse_log_message_t *msg;

    while (1) {
        for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
            if (hcnse_mutex_trylock(&(buf[i].mutex_deposit)) == HCNSE_OK) {

                msg = &(buf[log->rear].message);
                log->rear = ((log->rear) + 1) % HCNSE_LOG_BUF_SIZE;

                hcnse_mutex_unlock(&(buf[i].mutex_fetch));
                return msg;
            }
            else {
                continue;
            }
        }
    }
}

static hcnse_log_message_t *
hcnse_try_log_read(hcnse_log_t *log)
{
    hcnse_shared_record_t *buf = log->buf;
    hcnse_log_message_t *msg;
    int sleep_counter = 0;

    while (1) {
        for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
            if (hcnse_mutex_trylock(&(buf[i].mutex_fetch)) == HCNSE_OK) {

                msg = &(buf[log->front].message);
                log->front = ((log->front) + 1) % HCNSE_LOG_BUF_SIZE;

                hcnse_mutex_unlock(&(buf[i].mutex_deposit));
                return msg;
            }
            else {
                continue;
            }
        }
        sleep_counter += 1;
        if (sleep_counter == HCNSE_LOG_SLEEP_COUNTER) {
            hcnse_wait_wakeup_signal(HCNSE_LOG_SLEEP_TIME);
            sleep_counter = 0;
        }
    }
    return NULL;
}

static hcnse_thread_value_t
hcnse_log_worker(void *arg)
{
    hcnse_log_t *log = (hcnse_log_t *) arg;
    hcnse_log_message_t *msg;
    size_t maxlen = HCNSE_TIMESTRLEN + HCNSE_LOG_MSG_SIZE + sizeof(prio[0]);
    size_t len;
    char buf[maxlen];

    hcnse_msleep(HCNSE_LOG_WORKER_DELAY);

    printf("hcnse_log_worker()\n");

    while (1) {
        msg = hcnse_try_log_read(log);

        len = snprintf(buf, maxlen, "%s [%s] %s\n", 
                                msg->time, prio[msg->level], msg->str);

        /* TODO: Improve logs rotation */
        if (log->size) {
            if ((log->file->offset + len) > (log->size)) {
                log->file->offset = 0;
            }
        }

        if (hcnse_file_write1(log->file, buf, len) == -1) {
            /* what need to do? */
        }

        printf("%s", buf);
    }
    return 0;
}

/* Version for UNIX-like systems with mmap */
#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS) && (HCNSE_HAVE_MMAP)
hcnse_err_t
hcnse_log_init(hcnse_log_t **in_log, const char *fname, uint8_t level, size_t size)
{
    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    hcnse_shared_record_t *buf = MAP_FAILED;
    size_t mem_size;
    ssize_t file_size;
    uint32_t mutex_flags = HCNSE_MUTEX_PROCESS_SHARED;
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

    mem_size = sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE;

    buf = mmap(NULL, mem_size, PROT_READ|PROT_WRITE,
                                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) {
        err = hcnse_get_errno();
        goto failed;
    }

    for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
        err = hcnse_mutex_init(&(buf[i].mutex_deposit), mutex_flags);
        if (err != HCNSE_OK) {
            goto failed;
        }
        err = hcnse_mutex_init(&(buf[i].mutex_fetch), mutex_flags);
        if (err != HCNSE_OK) {
            goto failed;
        }
        hcnse_mutex_lock(&(buf[i].mutex_fetch));
    }

    log->file = file;
    log->level = level;
    log->size = size;
    log->buf = buf;

    /* XXX: Init all log struct fields before run worker */
    pid = fork();
    if (pid == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    if (!pid) {
        signal(SIGALRM, hcnse_log_sigalrm_handler);
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

    if (buf != MAP_FAILED) {
        munmap(buf, sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE);
    }

    return err;
}
#else
hcnse_err_t
hcnse_log_init(hcnse_log_t **in_log, const char *fname, uint8_t level, size_t size)
{
    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    hcnse_shared_record_t *buf = NULL;
    size_t mem_size;
    ssize_t file_size;
    uint32_t mutex_flags = 0;
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

    mem_size = sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE;

    buf = hcnse_malloc(mem_size);
    if (buf == NULL) {
        err = hcnse_get_errno();
        goto failed;
    }

    for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
        err = hcnse_mutex_init(&(buf[i].mutex_deposit), mutex_flags);
        if (err != HCNSE_OK) {
            goto failed;
        }
        err = hcnse_mutex_init(&(buf[i].mutex_fetch), mutex_flags);
        if (err != HCNSE_OK) {
            goto failed;
        }
        hcnse_mutex_lock(&(buf[i].mutex_fetch));
    }

    log->file = file;
    log->level = level;
    log->size = size;
    log->buf = buf;
    log->tid = tid;

#if !(HCNSE_WINDOWS)
    signal(SIGALRM, hcnse_log_sigalrm_handler);
#endif

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

    if (buf) {
        hcnse_free(buf);
    }

    return err;
}
#endif





void
hcnse_log_msg(uint8_t level, hcnse_log_t *log, const char *fmt, ...)
{
    hcnse_log_message_t *msg;
    va_list args;

    if (level > (log->level)) {
        return;
    }

    msg = hcnse_log_try_write(log);

    va_start(args, fmt);
    vsnprintf(msg->str, HCNSE_LOG_MSG_SIZE, fmt, args);
    va_end(args);

    hcnse_timestr(msg->time, HCNSE_TIMESTRLEN, time(NULL));
    msg->level = level;
#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS) && (HCNSE_HAVE_MMAP)
    hcnse_send_wakeup_signal1(log->pid);
#else
    hcnse_send_wakeup_signal(log->tid);
#endif
}

void
hcnse_log_error(uint8_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...)
{
    char *buf;
    size_t len;
    hcnse_log_message_t *msg;
    va_list args;

    if (level > (log->level)) {
        return;
    }

    msg = hcnse_log_try_write(log);

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
#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS) && (HCNSE_HAVE_MMAP)
    hcnse_send_wakeup_signal1(log->pid);
#else
    hcnse_send_wakeup_signal(log->tid);
#endif
}

void
hcnse_log_fini(hcnse_log_t *log)
{
    hcnse_shared_record_t *buf;
    buf = log->buf;

    hcnse_file_fini(log->file);

    for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
        hcnse_mutex_fini(&(buf[i].mutex_deposit));
        hcnse_mutex_fini(&(buf[i].mutex_fetch));
    }

#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS) && (HCNSE_HAVE_MMAP)
    kill(log->pid, SIGKILL);
    munmap(buf, sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE);
#else
    hcnse_thread_cancel(log->tid);
    hcnse_thread_destroy(log->tid);
    hcnse_free(buf);
    hcnse_free(log->tid);
#endif
    hcnse_free(log);
}
