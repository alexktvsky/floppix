#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "syshead.h"

#if (SYSTEM_WINDOWS)
#define PCRE_STATIC
#endif
#include <pcre.h>

#include "errors.h"
#include "connection.h"
#include "files.h"
#include "config.h"


#define PATTERN_LISTEN              0
#define PATTERN_LISTEN6             1
#define PATTERN_LOG                 2
#define PATTERN_LOGLEVEL            3
#define PATTERN_LOGSIZE             4
#define PATTERN_WORKDIR             5


#define FIRST_SUBSTR                (data + vector[2])
#define FIRST_SUBSTR_LEN            (vector[3] - vector[2])
#define SECOND_SUBSTR               (data + vector[4])
#define SECOND_SUBSTR_LEN           (vector[5] - vector[4])
#define THIRD_SUBSTR                (data + vector[6])
#define THIRD_SUBSTR_LEN            (vector[7] - vector[6])





static void config_set_default_params(config_t *conf)
{




#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
    conf->workdir = "/";
    // conf->log = "";
#elif (SYSTEM_WINDOWS)
    conf->workdir = "C:\\";
    // conf->log = "";
#endif
    conf->logsize = 0;
    conf->loglevel = 1;








    return;
}



err_t config_init(config_t **conf, const char *fname)
{

    config_t *new_conf = NULL;
    file_t *file = NULL;
    err_t err;

    new_conf = malloc(sizeof(config_t));
    if (!new_conf) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    // memset(new_conf, 0, sizeof(config_t));
    /* TODO: Set default value for config variables */
    config_set_default_params(new_conf);

    file = malloc(sizeof(file_t));
    if (!file) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    if (file_init(file, fname, SYS_FILE_RDONLY,
                    SYS_FILE_OPEN, SYS_FILE_DEFAULT_ACCESS) != OK) {
        err = ERR_FILE_OPEN;
        goto failed;
    }

    new_conf->file = file;
    *conf = new_conf;
    return OK;

failed:
    if (new_conf) {
        free(new_conf);
    }
    file_fini(file);
    if (file) {
        free(file);
    }
    return err;
}


void config_fini(config_t *conf)
{
    file_fini(conf->file);
    if (conf->file) {
        free(conf->file);
    }
    if (conf->data) {
        free(conf->data);
    }
    listening_t *temp1 = conf->listeners;
    listening_t *temp2;
    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;
        free(temp2);
    }
    return;
}


err_t config_parse(config_t *conf)
{
    static const char *patterns[] = {
        "(?<=listen )([0-9]+.[0-9]+.[0-9]+.[0-9]+):([0-9]+)\n", /* PATTERN_LISTEN   */
        "(?<=listen )\\[([0-9/a-f/A-F/:/.]*)\\]:([0-9]+)\n",    /* PATTERN_LISTEN6  */
        "(?<=log )([0-9/a-z/A-Z///./:/\\\\]+)\n",               /* PATTERN_LOG      */
        "(?<=loglevel )([0-9]+)\n",                             /* PATTERN_LOGLEVEL */
        "(?<=logsize )([0-9]+)\n",                              /* PATTERN_LOGSIZE  */
        "(?<=workdir )([0-9/a-z/A-Z///./:/\\\\]+)\n"            /* PATTERN_WORKDIR  */
    };

    size_t number_of_patterns = sizeof(patterns)/sizeof(char *);
    char *data = NULL;

    const char *error;
    int erroffset;
    int rc;
    int vector[100];
    ssize_t fsize;
    ssize_t bytes_read;
    pcre *re = NULL;
    int offset = 0;
    err_t err;

    listening_t *cur_listener = NULL;
    listening_t *prev;

    fsize = file_size(conf->file);
    if (fsize == -1) {
        err = ERR_FILE_SIZE;
        goto failed;
    }

    data = malloc(fsize * sizeof(char) + 1);
    if (!data) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    bytes_read = file_read(conf->file, (uint8_t *) data, fsize, 0);
    if (bytes_read != fsize) {
        err = ERR_FILE_READ;
        goto failed;
    }
    data[fsize] = '\0';

    for (size_t number = 0; number < number_of_patterns; number++) {
        offset = 0;
        re = pcre_compile(patterns[number], PCRE_MULTILINE, &error, &erroffset, 0);
        if (!re) {
            err = ERR_CONF_REGEX;
            goto failed;
        }
        while (1) {
            rc = pcre_exec(re, 0, data, fsize, offset, 0, vector, sizeof(vector));
            if (rc < 0) {
                break;
            }

            switch (number) {
            case PATTERN_LISTEN:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                prev = cur_listener;
                cur_listener = malloc(sizeof(listening_t));
                if (!cur_listener) {
                    cur_listener = prev;
                    err = ERR_MEM_ALLOC;
                    goto failed;
                }
                cur_listener->ip = FIRST_SUBSTR;
                cur_listener->port = atoi(SECOND_SUBSTR);
                cur_listener->is_ipv6 = false;
                cur_listener->next = prev;
                break;
            case PATTERN_LISTEN6:
                /* do smth */
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                prev = cur_listener;
                cur_listener = malloc(sizeof(listening_t));
                if (!cur_listener) {
                    cur_listener = prev;
                    err = ERR_MEM_ALLOC;
                    goto failed;
                }
                cur_listener->ip = FIRST_SUBSTR;
                cur_listener->port = atoi(SECOND_SUBSTR);
                cur_listener->is_ipv6 = true;
                cur_listener->next = prev;
                break;
            case PATTERN_LOG:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->log = FIRST_SUBSTR;
                break;
            case PATTERN_LOGLEVEL:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->loglevel = (uint8_t) atoi(FIRST_SUBSTR);
                break;
            case PATTERN_LOGSIZE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->logsize = (size_t) atoi(FIRST_SUBSTR);
                break;
            case PATTERN_WORKDIR:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->workdir = FIRST_SUBSTR;
                break;
            }
            offset = vector[1];
        }
        pcre_free(re);
    }
    conf->data = data;
    conf->listeners = cur_listener;
    return OK;

failed:
    if (re) {
        pcre_free(re);
    }
    if (data) {
        free(data);
    }
    while (cur_listener) {
        prev = cur_listener;
        cur_listener = cur_listener->next;
        free(prev);
    }
    return err;
}
