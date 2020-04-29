#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>

#include "os/memory.h"
#include "os/files.h"
#include "os/time.h"
#include "server/errors.h"
#include "server/log.h"

#define HCNSE_LOG_BUF_SIZE 32
#define HCNSE_LOG_MSG_SIZE 300

#define HCNSE_LOG_INIT_DELAY 500
#define HCNSE_LOG_WORKER_DELAY 1000

#define HCNSE_LOG_SLEEP_COUNTER 3
#define HCNSE_LOG_SLEEP_TIME 5000

/*
 * TODO:
 * - LOG MUST BE NONBLOCKING
 * - Save to log server configuration
 */


typedef struct {
    uint8_t level;
    char time[HCNSE_TIMESTRLEN];
    char str[HCNSE_LOG_MSG_SIZE];
} hcnse_log_message_t;

typedef struct {
    pthread_mutex_t mutex_deposit;
    pthread_mutex_t mutex_fetch;
    hcnse_log_message_t message;
} hcnse_shared_record_t;


struct hcnse_log_s {
    hcnse_file_t *file;
    uint8_t level;
    size_t size;
    hcnse_shared_record_t *buf;
    uint32_t front;
    uint32_t rear;
    pid_t pid;
};

static const char *prio[] = {
    "emerg",
    "error",
    "warning",
    "info",
    "debug"
};

static void
hcnse_log_alarm_handler(int sig) {
    (void) sig;
    signal(SIGALRM, hcnse_log_alarm_handler);
}

static hcnse_log_message_t *
hcnse_log_try_write(hcnse_log_t *log)
{
    hcnse_shared_record_t *buf = log->buf;
    hcnse_log_message_t *msg;

    while (1) {
        for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
            if (pthread_mutex_trylock(&(buf[i].mutex_deposit)) == 0) {

                msg = &(buf[log->rear].message);
                log->rear = ((log->rear) + 1) % HCNSE_LOG_BUF_SIZE;

                pthread_mutex_unlock(&(buf[i].mutex_fetch));
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
            if (pthread_mutex_trylock(&(buf[i].mutex_fetch)) == 0) {

                msg = &(buf[log->front].message);
                log->front = ((log->front) + 1) % HCNSE_LOG_BUF_SIZE;

                pthread_mutex_unlock(&(buf[i].mutex_deposit));
                return msg;
            }
            else {
                continue;
            }
        }
        sleep_counter += 1;
        if (sleep_counter == HCNSE_LOG_SLEEP_COUNTER) {
            hcnse_msleep(HCNSE_LOG_SLEEP_TIME);
            sleep_counter = 0;
        }
    }
    return NULL;
}

static void
hcnse_log_worker(hcnse_log_t *log)
{
    hcnse_log_message_t *msg;
    size_t maxlen = HCNSE_TIMESTRLEN + HCNSE_LOG_MSG_SIZE + sizeof(prio[0]);
    size_t len;
    char buf[maxlen];

    hcnse_msleep(HCNSE_LOG_WORKER_DELAY);

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
    }
}

hcnse_err_t
hcnse_log_init(hcnse_log_t **in_log, const char *fname, uint8_t level, size_t size)
{
    hcnse_log_t *log = NULL;
    hcnse_file_t *file = NULL;
    hcnse_shared_record_t *buf;
    size_t mem_size;
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

    mem_size = sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE;

    buf = mmap(NULL, mem_size, PROT_READ|PROT_WRITE,
                                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) {
        err = hcnse_get_errno();;
        goto failed;
    }

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) || 
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
        err = hcnse_get_errno();
        goto failed;
    }

    for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
        if (pthread_mutex_init(&(buf[i].mutex_deposit), &attr) ||
            pthread_mutex_init(&(buf[i].mutex_fetch), &attr)) {
            err = hcnse_get_errno();
            goto failed;
        }
        pthread_mutex_lock(&(buf[i].mutex_fetch));
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
        signal(SIGALRM, hcnse_log_alarm_handler);
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

    return err;
}

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
    kill(log->pid, SIGALRM);
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
    kill(log->pid, SIGALRM);
}

void
hcnse_log_fini(hcnse_log_t *log)
{
    hcnse_shared_record_t *buf;
    buf = log->buf;

    hcnse_file_fini(log->file);

    for (size_t i = 0; i < HCNSE_LOG_BUF_SIZE; i++) {
        pthread_mutex_destroy(&(buf[i].mutex_deposit));
        pthread_mutex_destroy(&(buf[i].mutex_fetch));
    }

    /* kill child process */
    kill(log->pid, SIGKILL);
    munmap(buf, sizeof(hcnse_shared_record_t) * HCNSE_LOG_BUF_SIZE);

    hcnse_free(log);
}
