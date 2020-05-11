#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_FIRST_SUBSTR             (buf + vector[4])
#define HCNSE_FIRST_SUBSTR_LEN         (vector[5] - vector[4])
#define HCNSE_SECOND_SUBSTR            (buf + vector[6])
#define HCNSE_SECOND_SUBSTR_LEN        (vector[7] - vector[6])
#define HCNSE_THIRD_SUBSTR             (buf + vector[8])
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

static hcnse_err_t
hcnse_config_parse(hcnse_conf_t *conf)
{
    size_t number_of_patterns = sizeof(patterns)/sizeof(char *);
    size_t vector_size = 100;
    int vector[vector_size];
    int offset = 0;

    hcnse_regex_compile_t rc[number_of_patterns];

    char *buf;
    char *data;
    ssize_t fsize;
    ssize_t bytes_read;

    char *ptr1;
    char *ptr2;

    hcnse_err_t err;


    fsize = hcnse_file_size(conf->file);
    if (fsize == -1) {
        return hcnse_get_errno();
    }

    buf = hcnse_palloc(conf->pool, fsize * sizeof(char) + 1);
    if (!buf) {
        return hcnse_get_errno();
    }
    data = hcnse_palloc(conf->pool,fsize * sizeof(char) + 1);
    if (!data) {
        return hcnse_get_errno();
    }

    bytes_read = hcnse_file_read(conf->file, (uint8_t *) buf, fsize, 0);
    if (bytes_read != fsize) {
        return hcnse_get_errno();
    }
    buf[fsize] = '\0';

    ptr1 = data;

    for (size_t i = 0; i < number_of_patterns; i++) {
        err = hcnse_regex_compile(&rc[i], patterns[i]);
        if (err != HCNSE_OK) {
            goto failed;
        }
    }

    for (size_t i = 0; i < number_of_patterns; i++) {

        offset = 0;

        while (1) {

            err = hcnse_regex_exec(&rc[i], buf, fsize, offset, vector, vector_size);
            if (err == HCNSE_DECLINED) {
                break;
            }

            switch (i) {
            case HCNSE_PATTERN_LISTEN:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                hcnse_list_push_back(conf->addr_and_port, "0.0.0.0");
                hcnse_list_push_back(conf->addr_and_port, ptr1);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_LISTEN4:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                ptr2 = ptr1 + HCNSE_FIRST_SUBSTR_LEN + 1;

                hcnse_memmove(ptr2, HCNSE_SECOND_SUBSTR, HCNSE_SECOND_SUBSTR_LEN);
                ptr2[HCNSE_SECOND_SUBSTR_LEN] = '\0';

                hcnse_list_push_back(conf->addr_and_port, ptr1);
                hcnse_list_push_back(conf->addr_and_port, ptr2);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + HCNSE_SECOND_SUBSTR_LEN + 2;
                break;

            case HCNSE_PATTERN_LISTEN6:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                ptr2 = ptr1 + HCNSE_FIRST_SUBSTR_LEN + 1;

                hcnse_memmove(ptr2, HCNSE_SECOND_SUBSTR, HCNSE_SECOND_SUBSTR_LEN);
                ptr2[HCNSE_SECOND_SUBSTR_LEN] = '\0';

                hcnse_list_push_back(conf->addr_and_port6, ptr1);
                hcnse_list_push_back(conf->addr_and_port6, ptr2);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + HCNSE_SECOND_SUBSTR_LEN + 2;
                break;

            case HCNSE_PATTERN_LOG_FILE:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->log_fname = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_LOG_SIZE:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->log_size = (size_t) hcnse_atoi(ptr1);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_LOG_REWRITE:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';
                if (!hcnse_strncmp(ptr1, "on", 2)) {
                    conf->log_rewrite = true;
                }
                else {
                    conf->log_rewrite = false;
                }

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
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
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->workdir = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_PRIORITY:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->priority = (int8_t) hcnse_atoi(ptr1);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_TIMER:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->timer = (hcnse_msec_t) hcnse_atoi(ptr1);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_USER:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->user = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_GROUP:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->group = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_WORKER_PROCESSES:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->worker_processes = (uint8_t) hcnse_atoi(ptr1);

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_WORKER_PROCESSES_AUTO:
                conf->worker_processes = 0;
                break;

            case HCNSE_PATTERN_SSL:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';
                if (!hcnse_strncmp(ptr1, "on", 2)) {
                    conf->ssl_on = true;
                }
                else {
                    conf->ssl_on = false;
                }

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_SSL_CERTFILE:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->ssl_certfile = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;

            case HCNSE_PATTERN_SSL_KEYFILE:
                hcnse_memmove(ptr1, HCNSE_FIRST_SUBSTR, HCNSE_FIRST_SUBSTR_LEN);
                ptr1[HCNSE_FIRST_SUBSTR_LEN] = '\0';

                conf->ssl_keyfile = ptr1;

                ptr1 += HCNSE_FIRST_SUBSTR_LEN + 1;
                break;
            }
            offset = vector[1];
        }
    }

    for (size_t i = 0; i < number_of_patterns; i++) {
        hcnse_regex_destroy(&rc[i]);
    }

    return HCNSE_OK;

failed:
    // for (size_t i = 0; i < number_of_patterns; i++) {
    //     hcnse_regex_destroy(&rc[i]);
    // }
    return err; 
}

hcnse_err_t
hcnse_config_create_and_parse(hcnse_conf_t **in_conf, const char *fname)
{
    hcnse_conf_t *conf;
    hcnse_pool_t *pool;
    hcnse_file_t *file;
    hcnse_list_t *addr_and_port;
    hcnse_list_t *addr_and_port6;
    hcnse_err_t err;

    pool = hcnse_pool_create(NULL);
    if (!pool) {
        err = hcnse_get_errno();
        goto failed;
    }

    conf = hcnse_pcalloc(pool, sizeof(hcnse_conf_t));
    if (!conf) {
        err = hcnse_get_errno();
        goto failed;
    }

    file = hcnse_pcalloc(pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, fname, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_register(pool, file, hcnse_file_fini);

    addr_and_port = hcnse_list_create(pool);
    if (!addr_and_port) {
        err = hcnse_get_errno();
        goto failed;
    }

    addr_and_port6 = hcnse_list_create(pool);
    if (!addr_and_port6) {
        err = hcnse_get_errno();
        goto failed;
    }


    conf->pool = pool;
    conf->file = file;
    conf->addr_and_port = addr_and_port;
    conf->addr_and_port6 = addr_and_port6;

    hcnse_config_set_default_params(conf);

    err = hcnse_config_parse(conf);
    if (err != HCNSE_OK) {
        goto failed;
    }

    *in_conf = conf;
    return HCNSE_OK;

failed:
    if (pool) {
        hcnse_pool_destroy(pool);
    }
    return err;
}

void
hcnse_config_destroy(hcnse_conf_t *conf)
{
    hcnse_pool_destroy(conf->pool);
}
