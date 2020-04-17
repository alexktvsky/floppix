#include <stdlib.h>
#include <string.h>

#include "syshead.h"

#if (SYSTEM_WINDOWS)
#define PCRE_STATIC
#endif
#include <pcre.h>

#include "config.h"


#define PATTERN_LISTEN              0
#define PATTERN_LISTEN4             1
#define PATTERN_LISTEN6             2
#define PATTERN_LOG_FILE            3
#define PATTERN_LOG_LEVEL           4
#define PATTERN_LOG_SIZE            5
#define PATTERN_WORKDIR             6
#define PATTERN_SSL_ON              7
#define PATTERN_SSL_OFF             8
#define PATTERN_SSL_CERTFILE        9
#define PATTERN_SSL_KEYFILE         10
#define PATTERN_PRIORITY            11


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
    conf->log_file = "server.log";

#elif (SYSTEM_WINDOWS)
    conf->workdir = "C:\\";
    conf->log_file = "server.log";
#endif

    conf->log_size = 0;
    conf->log_level = 1;
    conf->priority = 0;
    return;
}


static err_t config_parse(config_t *conf)
{
    static const char *patterns[] = {
        "(?<=listen )\\s*([0-9]+)\n",
        "(?<=listen )\\s*([0-9]+.[0-9]+.[0-9]+.[0-9]+):([0-9]+)\n",
        "(?<=listen )\\s*\\[([0-9/a-f/A-F/:/%%/.]*)\\]:([0-9]+)\n",
        "(?<=log_file )\\s*([\\S]+)\n",
        "(?<=log_level )\\s*([0-9]+)\n",
        "(?<=log_size )\\s*([0-9]+)\n",
        "(?<=workdir )\\s*([\\S]+)\n",
        "(?<=ssl )\\s*(on)\n",
        "(?<=ssl )\\s*(off)\n",
        "(?<=ssl_certfile )\\s*([\\S]+)\n",
        "(?<=ssl_keyfile )\\s*([\\S]+)\n",
        "(?<=priority )\\s*([0-9/-]+)\n"  // signed value
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

    listener_t *listener;

    fsize = file_size(conf->file);
    if (fsize == -1) {
        err = ERR_CONF_SIZE;
        goto failed;
    }

    data = malloc(fsize * sizeof(char) + 1);
    if (!data) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    bytes_read = file_read(conf->file, (uint8_t *) data, fsize, 0);
    if (bytes_read != fsize) {
        err = ERR_CONF_READ;
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

                listener = list_create_node_and_append(listener_t, conf->listeners);
                if (!listener) {
                    goto failed;
                }

                err = listener_init_ipv4(listener, "0.0.0.0", FIRST_SUBSTR);
                if (err != OK) {
                    goto failed;
                }
                break;

            case PATTERN_LISTEN4:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                listener = list_create_node_and_append(listener_t, conf->listeners);
                if (!listener) {
                    goto failed;
                }

                err = listener_init_ipv4(listener, FIRST_SUBSTR, SECOND_SUBSTR);
                if (err != OK) {
                    goto failed;
                }
                break;

            case PATTERN_LISTEN6:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';

                listener = list_create_node_and_append(listener_t, conf->listeners);
                if (!listener) {
                    goto failed;
                }

                err = listener_init_ipv6(listener, FIRST_SUBSTR, SECOND_SUBSTR);
                if (err != OK) {
                    goto failed;
                }
                break;

            case PATTERN_LOG_FILE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->log_file = FIRST_SUBSTR;
                break;

            case PATTERN_LOG_LEVEL:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->log_level = (uint8_t) atoi(FIRST_SUBSTR);
                break;

            case PATTERN_LOG_SIZE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->log_size = (size_t) atoi(FIRST_SUBSTR);
                break;

            case PATTERN_WORKDIR:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->workdir = FIRST_SUBSTR;
                break;

            case PATTERN_SSL_ON:
                conf->ssl_on = true;
                break;

            case PATTERN_SSL_OFF:
                conf->ssl_on = false;
                break;

            case PATTERN_SSL_CERTFILE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->ssl_certfile = FIRST_SUBSTR;
                break;

            case PATTERN_SSL_KEYFILE:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->ssl_keyfile = FIRST_SUBSTR;
                break;

            case PATTERN_PRIORITY:
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                conf->priority = (int8_t) atoi(FIRST_SUBSTR);
                break;
            }
            offset = vector[1];
        }
        pcre_free(re);
    }
    conf->data = data;

    return OK;

failed:
    if (re) {
        pcre_free(re);
    }
    if (data) {
        free(data);
    }
    return err;
}


err_t config_init(config_t **conf, const char *fname)
{
    config_t *new_conf = NULL;
    file_t *new_file = NULL;
    list_t *listeners = NULL;
    list_t *free_connects = NULL;
    err_t err;

    new_conf = malloc(sizeof(config_t));
    if (!new_conf) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }
    memset(new_conf, 0, sizeof(config_t));

    new_file = malloc(sizeof(file_t));
    if (!new_file) {
        err = ERR_MEM_ALLOC;
        goto failed;
    }

    err = list_create1(&listeners);
    if (err != OK) {
        goto failed;
    }
    err = list_create1(&free_connects);
    if (err != OK) {
        goto failed;
    }

    if (file_init(new_file, fname, SYS_FILE_RDONLY,
                    SYS_FILE_OPEN, SYS_FILE_DEFAULT_ACCESS) != OK) {
        err = ERR_CONF_OPEN;
        goto failed;
    }

    new_conf->file = new_file;
    new_conf->listeners = listeners;
    new_conf->free_connects = free_connects;

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
    file_fini(new_file);
    if (new_file) {
        free(new_file);
    }
    if (listeners) {
        list_destroy(listeners);
    }
    if (free_connects) {
        list_destroy(free_connects);
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
    // Do we need clean listeners in list? */
    list_destroy(conf->listeners);
    list_destroy(conf->free_connects);
    free(conf);
    return;
}
