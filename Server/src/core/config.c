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
#include "files.h"
#include "config.h"


#define PATTERN_LISTEN              0
#define PATTERN_LISTEN6             1
#define PATTERN_LOG                 2
#define PATTERN_LOGLEVEL            3
#define PATTERN_LOGSIZE             4


#define FIRST_SUBSTR                (data + vector[2])
#define FIRST_SUBSTR_LEN            (vector[3] - vector[2])
#define SECOND_SUBSTR               (data + vector[4])
#define SECOND_SUBSTR_LEN           (vector[5] - vector[4])
#define THIRD_SUBSTR                (data + vector[6])
#define THIRD_SUBSTR_LEN            (vector[7] - vector[6])


err_t config_init(config_t *conf, const char *fname)
{
    file_t *file = NULL;
    err_t err;

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

    conf->file = file;
    return OK;

failed:
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
    return;
}


err_t config_do_parse(config_t *conf)
{
    static const char *patterns[] = {
        "(?<=listen )([0-9]+.[0-9]+.[0-9]+.[0-9]+):([0-9]+)\n", /* PATTERN_LISTEN   */
        "(?<=listen )\\[([0-9/a-f/A-F/:]*)\\]:([0-9]+)\n",      /* PATTERN_LISTEN6  */
        "(?<=log )([0-9/a-z/A-Z///./:/\\\\]+)\n",               /* PATTERN_LOG      */
        "(?<=loglevel )([0-9]+)\n",                             /* PATTERN_LOGLEVEL */
        "(?<=logsize )([0-9]+)\n"                               /* PATTERN_LOGSIZE  */
    };

    size_t number_of_patterns = sizeof(patterns)/sizeof(char *);
    const char *error;
    char *data = NULL;
    int erroffset;
    int rc;
    int vector[100];
    ssize_t fsize;
    ssize_t bytes_read;
    pcre *re = NULL;
    int offset = 0;
    err_t err;

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
                /* do smth */
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';
                printf("%s\n", FIRST_SUBSTR);
                printf("%s\n", SECOND_SUBSTR);
                break;
            case PATTERN_LISTEN6:
                /* do smth */
                FIRST_SUBSTR[FIRST_SUBSTR_LEN] = '\0';
                SECOND_SUBSTR[SECOND_SUBSTR_LEN] = '\0';
                printf("%s\n", FIRST_SUBSTR);
                printf("%s\n", SECOND_SUBSTR);
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


int main(void)
{
    err_t err;

    config_t *conf = malloc(sizeof(config_t));

    err = config_init(conf, "../../sample/sample-config-files/server.conf");
    if (err != OK) {
        printf("%s\n", set_strerror(err));
        free(conf);
        abort();
    }

    err = config_do_parse(conf);
    if (err != OK) {
        printf("%s\n", set_strerror(err));
        free(conf);
        abort();
    }

    printf("%s\n", conf->log);
    printf("%d\n", conf->loglevel);
    printf("%ld\n", conf->logsize);

    config_fini(conf);
    free(conf);
    return 0;
}
