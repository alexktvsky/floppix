#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

#include "../inc/error_proc.h"
#include "../inc/pools.h"
#include "../inc/log.h"

#define MAX_STRLEN_TIME 50
#define MAX_STRLEN_MESSAGE 100
#define MAX_STRLEN_LOGNAME 1000

#define LOGGER_AWAIT_END 10000


typedef struct message_unit_s message_unit_t;

struct message_unit_s {
    char time[MAX_STRLEN_TIME];
    int level;
    char message[MAX_STRLEN_MESSAGE];
    message_unit_t *next;
};

static int minlevel = 0;
static char logname[MAX_STRLEN_LOGNAME];
static message_unit_t *logpool = NULL;
static pthread_mutex_t mutex;
static _Atomic int log_enable = 0;
static pool_t *pool;

static const char *priorities[] = {
    "error",
    "warning",
    "info",
    "debug"
};


static void *logger(void *filename)
{
    filename = (char *) filename;
    FILE *logfile = fopen(filename, "a");
    if (!logfile) {
        pthread_exit(NULL);
    }

#if defined (__gnu_linux__)
        struct stat statbuf;
#elif defined (__WIN32__) || defined (__WIN64__)
        struct _stat statbuf;
#endif

    while (log_enable) {
        /* If log pool is not empty */
        if (logpool) {

            if ((logpool->level) <= minlevel) {
#if defined (__gnu_linux__)
                if (stat(logname, &statbuf))
#elif defined (__WIN32__) || defined (__WIN64__)
                if (_stat(logname, &statbuf))
#endif
                {
                    pthread_exit(NULL);
                }

                if (fprintf(logfile, "%s [%s] %s\n",
                            logpool->time,
                            priorities[logpool->level],
                            logpool->message) < 0) {
                    log_enable = 0;
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }

                if (fflush(logfile) == EOF) {
                    log_enable = 0;
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }
            }
            logpool = logpool->next;
            //free(temp);
            pthread_mutex_unlock(&mutex);
        }
        else {
            if (!cleaned) {
                pool_clear(pool);
                cleaned = 1;
            }
            pthread_mutex_unlock(&mutex);
        }
    }
    fclose(logfile);
    pool_destroy(pool);
}


status_t init_log(char *filename, int level_boundary)
{
    int len_name = hcnse_strlen(filename);
    if (len_name > MAX_STRLEN_LOGNAME) {
        return 1;
    }
    hcnse_memmove(logname, filename, len_name);
    logname[len_name] = '\0';
    minlevel = level_boundary;

    pthread_t hcnse_log_tid;
    pthread_attr_t hcnse_log_tattr;
    pthread_attr_init(&hcnse_log_tattr);
    pthread_attr_setscope(&hcnse_log_tattr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&hcnse_log_tid, NULL, logger, (void *) filename)) {
        return 2;
    }
    pthread_detach(hcnse_log_tid);
    log_enable = 1;
    return HCNSE_OK;
}


status_t log_message(int level, char *message)
{
    if (!log_enable) {
        return 1;
    }
    char strtime[MAX_STRLEN_TIME];
    time_t timer = time(NULL);
    int timelen;
    int msglen;
    timelen = strftime(strtime, MAX_STRLEN_TIME,
                   "[%d.%m.%Y] [%H:%M:%S]", localtime(&timer));
    if (!timelen) {
        return 2;
    }

    message_unit_s *node = hcnse_palloc(pool, sizeof(message_unit_s));
    pthread_mutex_lock(&mutex);
    hcnse_memmove(node->time, strtime, timelen);
    (node->time)[timelen] = '\0';
    node->level = level;
    msglen = hcnse_strlen(message);
    if (msglen > MAX_STRLEN_MESSAGE) {
        msglen = MAX_STRLEN_MESSAGE;
    }
    hcnse_memmove(node->message, message, msglen);
    (node->message)[msglen] = '\0';
    node->next = NULL;

    message_unit_s *temp1 = logpool;
    message_unit_s *temp2;
    /* If log pool is empty */
    if (!temp1) {
        logpool = node;
    }
    /* If log pool is not empty */
    else {
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->next;
        }
        temp2->next = node;
    }
    pthread_mutex_unlock(&mutex);
    return HCNSE_OK;
}


status_t log_error(status_t statcode)
{
    char buf[MAX_STRLEN_MESSAGE];
    snprintf(buf, MAX_STRLEN_MESSAGE, "Error %d-%d. %s", 
             SET_ERROR_DOMAIN(statcode),
             statcode,
             set_strerror(statcode));
    status_t stat = log_message(LOG_ERROR, buf);
    if (stat != HCNSE_OK) {
        return stat;
    }
}


void fini_log(void)
{
    log_enable = 0;
    usleep(LOGGER_AWAIT_END);
/*
    if (logpool) {
        message_unit_s *temp1 = logpool;
        message_unit_s *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->next;
            free(temp2);
        }
    }
*/
}


int main(void)
{
    init_log("server.log", LOG_INFO);
    usleep(1000000);

    for (int i = 0; i < 100; i++) {
        log_error(CONF_SYNTAX_ERROR);
    }
    printf("%d\n", get_pool_free_size(pool));
    usleep(1000000);
    printf("%d\n", get_pool_free_size(pool));

    fini_log();
    usleep(10000);
    return 0;
}
