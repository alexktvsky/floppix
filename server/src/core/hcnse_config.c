#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_FIRST_SUBSTR             (raw_data + vector[4])
#define HCNSE_FIRST_SUBSTR_LEN         (vector[5] - vector[4])
#define HCNSE_SECOND_SUBSTR            (raw_data + vector[6])
#define HCNSE_SECOND_SUBSTR_LEN        (vector[7] - vector[6])
#define HCNSE_THIRD_SUBSTR             (raw_data + vector[8])
#define HCNSE_THIRD_SUBSTR_LEN         (vector[9] - vector[8])

#define HCNSE_COMMIT_BEFORE            "^(?!#)\\s*"
#define HCNSE_COMMIT_AFTER             "([\\s]*|[\\s]+#.*)"
#define HCNSE_LF_OR_CRLF               "(\n|\r\n)"
#define HCNSE_REGEX_STR(str) \
    (HCNSE_COMMIT_BEFORE str HCNSE_COMMIT_AFTER HCNSE_LF_OR_CRLF)

typedef enum {
    HCNSE_PATTERN_LISTEN,
    HCNSE_PATTERN_LISTEN4,
    HCNSE_PATTERN_LISTEN6,

    HCNSE_PATTERN_LOG_FILE,
    HCNSE_PATTERN_LOG_SIZE,
    HCNSE_PATTERN_LOG_REWRITE,

    /* Log level */
    HCNSE_PATTERN_LOG_EMERG,
    HCNSE_PATTERN_LOG_ERROR,
    HCNSE_PATTERN_LOG_WARN,
    HCNSE_PATTERN_LOG_INFO,
    HCNSE_PATTERN_LOG_DEBUG,

    HCNSE_PATTERN_WORKDIR,
    HCNSE_PATTERN_PRIORITY,
    HCNSE_PATTERN_TIMER,
    HCNSE_PATTERN_USER,
    HCNSE_PATTERN_GROUP,

    HCNSE_PATTERN_WORKER_PROCESSES,
    HCNSE_PATTERN_WORKER_PROCESSES_AUTO,

    HCNSE_PATTERN_SSL,
    HCNSE_PATTERN_SSL_CERTFILE,
    HCNSE_PATTERN_SSL_KEYFILE,
} hcnse_pattern_number_t;


static const char *patterns[] = {
    HCNSE_REGEX_STR("(listen)\\s+([0-9]+)(?!\\.)"),
    HCNSE_REGEX_STR("(listen)\\s+([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+):([0-9]+)"),
    HCNSE_REGEX_STR("(listen)\\s+\\[([0-9/a-z/A-Z/:/%%/.]*)\\]:([0-9]+)"),

    HCNSE_REGEX_STR("(log_file)\\s+([\\S]+)"),
    HCNSE_REGEX_STR("(log_size)\\s+([0-9]+)"),
    HCNSE_REGEX_STR("(log_rewrite)\\s+(on|off)"),

    HCNSE_REGEX_STR("(log_level)\\s+(emerg)"),
    HCNSE_REGEX_STR("(log_level)\\s+(error)"),
    HCNSE_REGEX_STR("(log_level)\\s+(warn)"),
    HCNSE_REGEX_STR("(log_level)\\s+(info)"),
    HCNSE_REGEX_STR("(log_level)\\s+(debug)"),

    HCNSE_REGEX_STR("(workdir)\\s+([\\S]+)"),
    HCNSE_REGEX_STR("(priority)\\s+([0-9/-]+)"),  // signed value
    HCNSE_REGEX_STR("(timer)\\s+([0-9]+)"),
    HCNSE_REGEX_STR("(user)\\s+([\\S]+)"),
    HCNSE_REGEX_STR("(group)\\s+([\\S]+)"),

    HCNSE_REGEX_STR("(worker_processes)\\s+([0-9]+)"),
    HCNSE_REGEX_STR("(log_level)\\s+(auto)"),

    HCNSE_REGEX_STR("(ssl)\\s+(on|off)"),
    HCNSE_REGEX_STR("(ssl_certfile)\\s+([\\S]+)"),
    HCNSE_REGEX_STR("(ssl_keyfile)\\s+([\\S]+)")
};


static void
hcnse_config_set_default_params(hcnse_conf_t *conf)
{
#if (HCNSE_POSIX)
    conf->workdir = "/";
    conf->log_fname = "server.log";

#elif (HCNSE_WIN32)
    conf->workdir = "C:\\";
    conf->log_fname = "server.log";
#endif

    conf->log_size = 0;
    conf->log_level = HCNSE_LOG_ERROR;
    conf->log_rewrite = false;

    conf->priority = 0;
    conf->timer = 30000;

    conf->worker_processes = 0;
}

static void *
hcnse_select_pattern(hcnse_conf_t *conf, char *raw_data,
    char *data, size_t fsize, size_t pattern)
{
    size_t vector_size = 100;
    int vector[vector_size];
    int offset = 0;
    pcre *re = NULL;
    const char *error;
    int erroffset;
    int rc;
    hcnse_listener_t *listener;
    char *ptr;
    char *ptr2;
    hcnse_err_t err;

    ptr = data;

    re = pcre_compile(patterns[pattern], PCRE_MULTILINE, &error, &erroffset, 0);
    if (!re) {
        err = HCNSE_ERR_CONF_REGEX;
        goto failed;
    }
    while (1) {
        rc = pcre_exec(re, 0, raw_data, fsize, offset, 0, vector, vector_size);
        if (rc < 0) {
            break;
        }

        switch (pattern) {
        case HCNSE_PATTERN_LISTEN:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            listener = hcnse_list_create_node_and_append(
                sizeof(hcnse_listener_t), conf->listeners);
            if (!listener) {
                goto failed;
            }

            err = hcnse_listener_init_ipv4(listener, "0.0.0.0", ptr);
            if (err != HCNSE_OK) {
                goto failed;
            }

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LISTEN4:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            ptr2 = ptr + HCNSE_FIRST_SUBSTR_LEN + 1;

            hcnse_memmove(ptr2, HCNSE_SECOND_SUBSTR, HCNSE_SECOND_SUBSTR_LEN);
            ptr2[HCNSE_SECOND_SUBSTR_LEN] = '\0';

            listener = hcnse_list_create_node_and_append(
                sizeof(hcnse_listener_t), conf->listeners);
            if (!listener) {
                goto failed;
            }

            err = hcnse_listener_init_ipv4(listener, ptr, ptr2);
            if (err != HCNSE_OK) {
                goto failed;
            }

            ptr += HCNSE_FIRST_SUBSTR_LEN + HCNSE_SECOND_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LISTEN6:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            ptr2 = ptr + HCNSE_FIRST_SUBSTR_LEN + 1;

            hcnse_memmove(ptr2, HCNSE_SECOND_SUBSTR, HCNSE_SECOND_SUBSTR_LEN);
            ptr2[HCNSE_SECOND_SUBSTR_LEN] = '\0';

            listener = hcnse_list_create_node_and_append(
                sizeof(hcnse_listener_t), conf->listeners);
            if (!listener) {
                goto failed;
            }

            err = hcnse_listener_init_ipv6(listener, ptr, ptr2);
            if (err != HCNSE_OK) {
                goto failed;
            }

            ptr += HCNSE_FIRST_SUBSTR_LEN + HCNSE_SECOND_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LOG_FILE:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->log_fname = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LOG_SIZE:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->log_size = (size_t) hcnse_atoi(ptr);

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LOG_REWRITE:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';
            if (!hcnse_strncmp(ptr, "on", 2)) {
                conf->log_rewrite = true;
            }
            else {
                conf->log_rewrite = false;
            }

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_LOG_EMERG:
            conf->log_level = HCNSE_LOG_EMERG;
            break;

        case HCNSE_PATTERN_LOG_ERROR:
            conf->log_level = HCNSE_LOG_ERROR;
            break;

        case HCNSE_PATTERN_LOG_WARN:
            conf->log_level = HCNSE_LOG_WARN;
            break;

        case HCNSE_PATTERN_LOG_INFO:
            conf->log_level = HCNSE_LOG_INFO;
            break;

        case HCNSE_PATTERN_LOG_DEBUG:
            conf->log_level = HCNSE_LOG_DEBUG;
            break;

        case HCNSE_PATTERN_WORKDIR:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->workdir = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_PRIORITY:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->priority = (int8_t) hcnse_atoi(ptr);

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_TIMER:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->timer = (hcnse_msec_t) hcnse_atoi(ptr);

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_USER:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->user = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_GROUP:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->group = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_WORKER_PROCESSES:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->worker_processes = (uint8_t) hcnse_atoi(ptr);

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_WORKER_PROCESSES_AUTO:
            conf->worker_processes = 0;
            break;

        case HCNSE_PATTERN_SSL:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';
            if (!hcnse_strncmp(ptr, "on", 2)) {
                conf->ssl_on = true;
            }
            else {
                conf->ssl_on = false;
            }

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_SSL_CERTFILE:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->ssl_certfile = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;

        case HCNSE_PATTERN_SSL_KEYFILE:
            hcnse_memmove(ptr, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
            ptr[HCNSE_FIRST_SUBSTR_LEN] = '\0';

            conf->ssl_keyfile = ptr;

            ptr += HCNSE_FIRST_SUBSTR_LEN + 1;
            break;
        }
        offset = vector[1];
    }
    pcre_free(re);

    return ptr;
    
failed:
    if (re) {
        pcre_free(re);
    }
    return NULL;
}

static hcnse_err_t
hcnse_config_parse(hcnse_conf_t *conf)
{
    size_t number_of_patterns = sizeof(patterns)/sizeof(char *);
    char *raw_data = NULL;
    char *data = NULL;
    ssize_t fsize;
    ssize_t bytes_read;
    void *ptr;
    hcnse_err_t err;

    fsize = hcnse_file_size(conf->file);
    if (fsize == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    raw_data = hcnse_malloc(fsize * sizeof(char) + 1);
    if (!raw_data) {
        err = hcnse_get_errno();
        goto failed;
    }
    data = hcnse_malloc(fsize * sizeof(char) + 1);
    if (!data) {
        err = hcnse_get_errno();
        goto failed;
    }

    bytes_read = hcnse_file_read(conf->file, (uint8_t *) raw_data, fsize, 0);
    if (bytes_read != fsize) {
        err = hcnse_get_errno();
        goto failed;
    }
    raw_data[fsize] = '\0';

    ptr = data;

    for (size_t pattern = 0; pattern < number_of_patterns; pattern++) {
        ptr = hcnse_select_pattern(conf, raw_data, ptr, fsize, pattern);
        if (!ptr) {
            goto failed;
        }
    }
    conf->data = data;

    hcnse_free(raw_data);
    return HCNSE_OK;

failed:
    if (raw_data) {
        hcnse_free(raw_data);
    }
    if (data) {
        hcnse_free(data);
    }
    return err;
}

hcnse_err_t
hcnse_config_create(hcnse_conf_t **in_conf, const char *fname)
{
    hcnse_conf_t *conf = NULL;
    hcnse_file_t *file = NULL;
    hcnse_list_t *listeners = NULL;
    hcnse_list_t *free_connects = NULL;
    hcnse_err_t err;

    conf = hcnse_malloc(sizeof(hcnse_conf_t));
    if (!conf) {
        err = hcnse_get_errno();
        goto failed;
    }
    hcnse_memset(conf, 0, sizeof(hcnse_conf_t));

    file = hcnse_malloc(sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_list_create1(&listeners);
    if (err != HCNSE_OK) {
        goto failed;
    }
    err = hcnse_list_create1(&free_connects);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_file_init(file, fname, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    conf->file = file;
    conf->listeners = listeners;
    conf->free_connects = free_connects;

    hcnse_config_set_default_params(conf);

    err = hcnse_config_parse(conf);
    if (err != HCNSE_OK) {
        goto failed;
    }

    *in_conf = conf;
    return HCNSE_OK;

failed:
    if (conf) {
        hcnse_free(conf);
    }
    hcnse_file_fini(file);
    if (file) {
        hcnse_free(file);
    }
    if (listeners) {
        hcnse_list_destroy(listeners);
    }
    if (free_connects) {
        hcnse_list_destroy(free_connects);
    }
    return err;
}

void
hcnse_config_fini(hcnse_conf_t *conf)
{
    hcnse_file_fini(conf->file);
    if (conf->file) {
        hcnse_free(conf->file);
    }
    if (conf->data) {
        hcnse_free(conf->data);
    }
    // Do we need clean listeners in list? */
    hcnse_list_destroy(conf->listeners);
    hcnse_list_destroy(conf->free_connects);
    hcnse_free(conf);
}
