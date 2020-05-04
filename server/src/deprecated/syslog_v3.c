#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#include "platform.h"
#include "error_proc.h"
#include "syslog.h"

#define MAX_STRLEN_TIME 50
#define MAX_STRLEN_MESSAGE 100
#define MAX_STR_FILENAME 1000
#define END_DELAY_TIME 1000000
#define BUFSIZE 10

typedef struct message_unit_s message_unit_t;

struct message_unit_s {
    char strtime[MAX_STRLEN_TIME];
    int level;
    char message[MAX_STRLEN_MESSAGE];
    message_unit_t *next;
};

static pthread_t tid;
static pthread_attr_t tattr;

static char filename[MAX_STR_FILENAME];
static FILE *openfile;
static int level_boundary;
static _Atomic bool log_enable;

static message_unit_t *buf;
static int front = 0;
static int rear = 0;
static sem_t empty;
static sem_t full;
static pthread_mutex_t mutex_deposit;
static pthread_mutex_t mutex_fetch;

static const char *priorities[] = {
    "error",
    "warning",
    "info",
    "debug"
};


static void *loger(void *data)
{
    openfile = fopen(filename, "a");
    if (!(openfile)) {
        printf("Error!\n");
        pthread_exit(NULL);
    }

#if (HCNSE_LINUX)
    struct stat statbuf;
#elif (HCNSE_WIN32) || (HCNSE_WIN64)
    struct _stat statbuf;
#endif

    while (log_enable) {
#if (HCNSE_LINUX)
        if (stat(filename, &statbuf) != HCNSE_OK)
#elif (HCNSE_WIN32) || (HCNSE_WIN64)
        if (_stat(filename, &statbuf) != HCNSE_OK)
#endif
        {
            printf("Error!\n");
            pthread_exit(NULL);
        }

        /* Lock */
        sem_wait(&full);
        pthread_mutex_lock(&mutex_fetch);

        /* Data proccessing */
        if (fprintf(openfile, "%s [%s] %s\n",
                    buf[front].strtime,
                    priorities[buf[front].level],
                    buf[front].message) < 0) {
            printf("Error!\n");
            pthread_exit(NULL);
        }

        fflush(openfile);

        /* Go to next element */
        front = (front + 1) % BUFSIZE;

        /* Unlock */
        pthread_mutex_unlock(&mutex_fetch);
        sem_post(&empty);
    }

    fclose(openfile);
	pthread_exit(NULL);
}


status_t init_log(char *in_filename, int in_level_boundary)
{
    size_t hcnse_strlen_filename = hcnse_strlen(in_filename) + 1;
    if (hcnse_strlen_filename > MAX_STR_FILENAME) {
        return 1;
    }
    else {
        hcnse_memmove(filename, in_filename, hcnse_strlen_filename);
        (filename)[hcnse_strlen_filename] = '\0';
    }

    level_boundary = in_level_boundary;
    sem_init(&empty, 0, BUFSIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex_deposit, NULL);
    pthread_mutex_init(&mutex_fetch, NULL);

    buf = malloc(sizeof(message_unit_t) * BUFSIZE);

    pthread_attr_init(&tattr);
    pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

    if (pthread_create(&tid, NULL, loger, NULL)) {
        return 1;
    }
    //pthread_detach(tid);

    log_enable = true;
    return HCNSE_OK;
}


status_t log_message(int level, char *message)
{
    /* Prepare data */
    char strtime[MAX_STRLEN_TIME];
    time_t timer = time(NULL);
    size_t timelen;
    timelen = strftime(strtime, MAX_STRLEN_TIME,
                   "[%d.%m.%Y] [%H:%M:%S]", localtime(&timer));

    size_t msglen = hcnse_strlen(message);
    if (msglen > MAX_STRLEN_MESSAGE) {
        msglen = MAX_STRLEN_MESSAGE;
    }

    /* Lock */
    sem_wait(&empty);
    pthread_mutex_lock(&mutex_deposit);

    /* Set data */
    hcnse_memmove(buf[rear].strtime, strtime, timelen);
    hcnse_memmove(buf[rear].message, message, msglen);
    buf[rear].level = level;

    /* Go to next element */
    rear = (rear + 1) % BUFSIZE;

    /* Unlock */
    pthread_mutex_unlock(&mutex_deposit);
    sem_post(&full);
    return HCNSE_OK;
}


status_t log_error(status_t statcode)
{
    char buf[MAX_STRLEN_MESSAGE];
    snprintf(buf, MAX_STRLEN_MESSAGE, "Error %d-%d. %s", 
             SET_ERROR_DOMAIN(statcode),
             statcode,
             set_strerror(statcode));
	return log_message(LOG_ERROR, buf);
}


void fini_log(void)
{
    usleep(END_DELAY_TIME);
    log_enable = false;
    usleep(END_DELAY_TIME);
    free(buf);
    pthread_attr_destroy(&tattr);
}
