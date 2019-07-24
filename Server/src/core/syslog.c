#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "platform.h"
#include "error_proc.h"
#include "syslog.h"

#define MAX_STRLEN_TIME 50
#define MAX_STRLEN_MESSAGE 100

#define HEADER_MSG_SIZE MAX_STRLEN_TIME
#define UNLIMITED 0

static int level_boundary;
static FILE *openfile;
static size_t size_counter = 0;
static size_t max_size = 0;

static const char *priorities[] = {
    "error",
    "warning",
    "info",
    "debug"
};


status_t init_log(char *in_filename, ssize_t in_maxsize, int in_level_boundary) {
    if (!in_filename) {
        return FAILED;
    }
    openfile = fopen(in_filename, "w+");
    if (!openfile) {
    	return FAILED;
    }
    level_boundary = in_level_boundary;
    if (in_maxsize == -1) {
        max_size = 0;
    }
    else if (in_maxsize != -1 && 
                in_maxsize < (HEADER_MSG_SIZE + MAX_STRLEN_MESSAGE)) {
        return FAILED; /* MAXLOGSIZE_ERROR */
    }
    else {
        max_size = in_maxsize;
    }
    return SUCCESS;
}


status_t log_msg(int in_level, char *message) {
	if (in_level > level_boundary) {
		return SUCCESS;
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
        rewind(openfile);
        size_counter = 0;
    }

    if (fprintf(openfile, "%s [%s] %s\n",
                            strtime,
                            priorities[in_level],
                            message) < 0) {
    	return FAILED;
    }
    fflush(openfile);
    size_counter += msglen + HEADER_MSG_SIZE;
    return SUCCESS;
}


status_t log_error(status_t statcode) {
    char buf[MAX_STRLEN_MESSAGE];
    snprintf(buf, MAX_STRLEN_MESSAGE, "Error %d-%d. %s", 
             SET_ERROR_DOMAIN(statcode),
             statcode,
             set_strerror(statcode));
	return log_msg(LOG_ERROR, buf);
}


void fini_log(void) {
	fclose(openfile);
}
