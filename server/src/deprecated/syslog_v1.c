#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../inc/error_proc.h"
#include "../inc/pools.h"
#include "../inc/log.h"

#define MAX_STRLEN_TIME 50
#define MAX_STRLEN_LOGNAME 1000

static FILE *logfile = NULL;
static int log_level = 0;
static char logname[MAX_STRLEN_LOGNAME];

#ifdef _PTHREAD_H
static pthread_mutex_t mutex;
#endif


static const char *priorities[] = {
    "error",
    "warning",
    "info",
    "debug"};

#define LOG_ERROR 0
#define LOG_WARN  1
#define LOG_INFO  2
#define LOG_DEBUG 3


int init_log(char *filename, int level)
{
    hcnse_memmove(logname, filename, hcnse_strlen(filename));
    logname[hcnse_strlen(filename)] = '\0';
    log_level = level;
    logfile = fopen(filename, "a");
    if (!logfile) {
        return 1;
    }
    return 0;
}


int log_message(int level, char *message)
{
    if (level > log_level) {
        return 0;
    }
#ifdef _PTHREAD_H
    pthread_mutex_lock(&mutex);
#endif
    char strtime[MAX_STRLEN_TIME];
    time_t timer = time(NULL);
    int len;

#if defined (__gnu_linux__)
    struct stat statbuf;
#elif defined (__WIN32__) || defined (__WIN64__)
    struct _stat statbuf;
#endif

    len = strftime(strtime, MAX_STRLEN_TIME,
                   "[%d.%m.%Y] [%H:%M:%S]", localtime(&timer));
    if (!len) {
        return 1;
    }
    strtime[len] = '\0';

#if defined (__gnu_linux__)
    if (stat(logname, &statbuf))
#elif defined (__WIN32__) || defined (__WIN64__)
    if (_stat(logname, &statbuf))
#endif
    {
        return 2;
    }
    if (!logfile) {
        return 3;
    }

    if (fprintf(logfile, "%s [%s] %s\n", strtime, priorities[level], message) < 0) {
        return 4;
    }

    if (fflush(logfile) == EOF) {
        return 5;
    }
#ifdef _PTHREAD_H
    pthread_mutex_unlock(&mutex);
#endif
    return 0;
}


int fini_log(void)
{
    if (!logfile) {
        return 1;
    }

    fclose(logfile);
    return 0;
}


int reinit_log(char *filename, int level)
{
    if (fini_log()) {
        return 1;
    }
    if (init_log(filename, level)) {
        return 2;
    }
    return 0;
}

//log_error() {}



int main(void)
{
    init_log("server.log", LOG_INFO);
    log_message(LOG_INFO, "Message 1!");
    log_message(LOG_INFO, "Message 2!");
    log_message(LOG_INFO, "Message 3!");
    fini_log();

    return 0;
}
