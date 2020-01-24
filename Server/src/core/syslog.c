#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "platform.h"
#include "errors.h"
#include "syslog.h"

#define MAX_STRLEN_TIME 50
#define MAX_STRLEN_MESSAGE 100

#define HEADER_MSG_SIZE MAX_STRLEN_TIME
#define UNLIMITED 0


static int level_boundary;
static FILE *open_logfile = NULL;
static size_t size_counter = 0;
static size_t max_size = 0;

static const char *priorities[] = {
    "emergency",
    "error",
    "warning",
    "info",
    "debug"
};


xxx_err_t init_log(const char *in_fname, ssize_t in_maxsize, int in_level_boundary)
{
    /* If log is already has been opened don't change it */ 
    if (open_logfile) {
        return XXX_OK;
    }

    open_logfile = fopen(in_fname, "w");
    if (!open_logfile) {
        return LOG_OPEN_ERROR; /* Error while open log file */
    }
    level_boundary = in_level_boundary;
    if (in_maxsize == -1) {
        max_size = 0;
    }
    else if (in_maxsize != -1 && 
                in_maxsize < (HEADER_MSG_SIZE + MAX_STRLEN_MESSAGE)) {
        return LOG_MAXSIZE_ERROR; /* Size of log file is too small */
    }
    else {
        max_size = in_maxsize;
    }
    return XXX_OK;
}


xxx_err_t log_msg_ex(FILE *in_openfile, int in_level, const char *message)
{
    
    if (!in_openfile) {
        return XXX_FAILED;
    }
    if (in_level > level_boundary) {
        return XXX_OK;
    }

    char strtime[MAX_STRLEN_TIME];
    time_t timer = time(NULL);
    size_t timelen = strftime(strtime, MAX_STRLEN_TIME,
                   "[%d.%m.%Y] [%H:%M:%S]", localtime(&timer));
    strtime[timelen] = '\0';

    size_t msglen = strlen(message);
    if (msglen > MAX_STRLEN_MESSAGE) {
        msglen = MAX_STRLEN_MESSAGE;
    }

    if ((size_counter + msglen + HEADER_MSG_SIZE) > max_size &&
        max_size != UNLIMITED) {
        rewind(in_openfile);
        size_counter = 0;
    }

    if (fprintf(in_openfile, "%s [%s] %s\n",
                            strtime,
                            priorities[in_level],
                            message) < 0) {
        return LOG_WRITE_ERROR; /* Error of writing log file */
    }
    fflush(in_openfile);
    size_counter += msglen + HEADER_MSG_SIZE;
    return XXX_OK;
}


xxx_err_t log_msg(int in_level, const char *message)
{
    return log_msg_ex(open_logfile, in_level, message);
}


xxx_err_t log_status(int in_level, xxx_err_t errcode)
{
    if (errcode == XXX_OK) {
        return XXX_OK;
    }
    char buf[MAX_STRLEN_MESSAGE];
    snprintf(buf, MAX_STRLEN_MESSAGE, "Error %d. %s", 
            errcode,
            set_strerror(errcode));
    return log_msg(in_level, buf);
}

void log_error(int level, const char *description, xxx_err_t errcode)
{
    log_msg(level, description);
    log_status(level, errcode);
}


void fini_log(void)
{
    if (open_logfile) {
        fclose(open_logfile);
        open_logfile = NULL;
    }
}
