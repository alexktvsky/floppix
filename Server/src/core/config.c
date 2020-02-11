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
#include "syslog.h"
#include "config.h"


#define PATTERN_LISTEN              0
#define PATTERN_LISTEN4             1
#define PATTERN_LISTEN6             2
#define PATTERN_LOGFILE             3
#define PATTERN_LOGLEVEL            4
#define PATTERN_LOGSIZE             5
#define PATTERN_WORKDIR             6
#define PATTERN_SSL_CERT            7
#define PATTERN_SSL_CERT_KEY        8


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
    conf->logfile = "server.log";

#elif (SYSTEM_WINDOWS)
    conf->workdir = "C:\\";
    conf->logfile = "server.log";
#endif

    conf->logsize = 0;
    conf->loglevel = 1;
    return;
}


static err_t config_parse(config_t *conf)
{
    static const char *patterns[] = {
        "(?<=listen )\\s*([0-9]+)\n",
        "(?<=listen )\\s*([0-9]+.[0-9]+.[0-9]+.[0-9]+):([0-9]+)\n",
        "(?<=listen )\\s*\\[([0-9/a-f/A-F/:/.]*)\\]:([0-9]+)\n",
        "(?<=logfile )\\s*([\\S]+)\n",
        "(?<=loglevel )\\s*([0-9]+)\n",
        "(?<=logsize )\\s*([0-9]+)\n",
        "(?<=workdir )\\s*([\\S]+)\n",
        "(?<=ssl_certificate )\\s*([\\S]+)\n",
        "(?<=ssl_certificate_key )\\s*([\\S]+)\n"
    };

    size_t number_of_patterns = sizeof(patterns)/sizeof(char *);
    char *data = NULL;

    const char *error;
    int erroffset;
    int rc;
    size_t vector_size = 100;
    int vector[vector_size];
    ssize_t fsize;
    ssize_t bytes_read;
    pcre *re = NULL;
    int offset = 0;
    err_t err;

    listener_t *cur_listener = NULL;
    listener_t *prev_listener;

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
            rc = pcre_exec(re, 0, data, fsize, offset, 0, vector, vector_size);
            if (rc < 0) {
                break;
            }

            switch (number) {
            case PATTERN_LISTEN:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';

                prev_listener = cur_listener;
                cur_listener = malloc(sizeof(listener_t));
                if (!cur_listener) {
                    cur_listener = prev_listener;
                    err = ERR_MEM_ALLOC;
                    goto failed;
                }
                cur_listener->ip = "0.0.0.0";
                cur_listener->port = (uint16_t) atoi(FIRST_SUBSTR);
                cur_listener->is_ipv6 = false;
                cur_listener->next = prev_listener;
                break;

            case PATTERN_LISTEN4:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                prev_listener = cur_listener;
                cur_listener = malloc(sizeof(listener_t));
                if (!cur_listener) {
                    cur_listener = prev_listener;
                    err = ERR_MEM_ALLOC;
                    goto failed;
                }
                cur_listener->ip = FIRST_SUBSTR;
                cur_listener->port = (uint16_t) atoi(SECOND_SUBSTR);
                cur_listener->is_ipv6 = false;
                cur_listener->next = prev_listener;
                break;

            case PATTERN_LISTEN6:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                prev_listener = cur_listener;
                cur_listener = malloc(sizeof(listener_t));
                if (!cur_listener) {
                    cur_listener = prev_listener;
                    err = ERR_MEM_ALLOC;
                    goto failed;
                }
                cur_listener->ip = FIRST_SUBSTR;
                cur_listener->port = (uint16_t) atoi(SECOND_SUBSTR);
                cur_listener->is_ipv6 = true;
                cur_listener->next = prev_listener;
                break;

            case PATTERN_LOGFILE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->logfile = FIRST_SUBSTR;
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

            case PATTERN_SSL_CERT:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->cert = FIRST_SUBSTR;
                break;

            case PATTERN_SSL_CERT_KEY:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->cert_key = FIRST_SUBSTR;
                break;
            }
            offset = vector[1];
        }
        pcre_free(re);
    }
    conf->data = data;
    conf->listeners = cur_listener;

    /* TODO: Log server configuration */
    printf("conffile = \"%s\"\n", conf->file->name);
    printf("logfile = \"%s\"\n", conf->logfile);
    printf("loglevel = %d\n", conf->loglevel);
    printf("logsize = %ld\n", conf->logsize);
    printf("workdir = \"%s\"\n", conf->workdir);
    printf("ssl_certificate = \"%s\"\n", conf->cert);
    printf("ssl_certificate_key = \"%s\"\n", conf->cert_key);
    listener_t *temp = conf->listeners;
    while (temp) {
        printf("listen %s:%d\n", temp->ip, temp->port);
        temp = temp->next;
    }

    return OK;

failed:
    if (re) {
        pcre_free(re);
    }
    if (data) {
        free(data);
    }
    while (cur_listener) {
        prev_listener = cur_listener;
        cur_listener = cur_listener->next;
        free(prev_listener);
    }
    return err;
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
    memset(new_conf, 0, sizeof(config_t));

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

    config_set_default_params(new_conf);

    err = config_parse(new_conf);
    if (err != OK) {
        goto failed;
    }

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
    listener_t *temp1 = conf->listeners;
    listener_t *temp2;
    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;
        free(temp2);
    }
    free(conf);
    return;
}
