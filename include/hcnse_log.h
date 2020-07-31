#ifndef INCLUDED_HCNSE_LOG_H
#define INCLUDED_HCNSE_LOG_H

#include "hcnse_portable.h"
#include "hcnse_common.h"

#define HCNSE_LOG_EMERG   0
#define HCNSE_LOG_ERROR   1
#define HCNSE_LOG_WARN    2
#define HCNSE_LOG_INFO    3
#define HCNSE_LOG_DEBUG   4

#define hcnse_log_set_global(log)      (hcnse_log_global = log)
#define hcnse_log_get_global()         (hcnse_log_global)

#define hcnse_log_stdout(err, ...) \
    hcnse_log_console(HCNSE_STDOUT, err, __VA_ARGS__)

#define hcnse_log_stderr(err, ...) \
    hcnse_log_console(HCNSE_STDERR, err, __VA_ARGS__)

#define hcnse_log_debug(log, err, ...) \
    hcnse_log_error(HCNSE_LOG_DEBUG, log, err, __VA_ARGS__)

#if (HCNSE_LOG_REDIRECT_TO_STDERR)
#define hcnse_log_error1(level, err, ...) \
    (hcnse_log_get_global() ? \
        hcnse_log_error(level, hcnse_log_get_global(), err, __VA_ARGS__) : \
        hcnse_log_stderr(err, __VA_ARGS__))

#define hcnse_log_debug1(err, ...) \
    (hcnse_log_get_global() ? \
        hcnse_log_debug(hcnse_log_get_global(), err, __VA_ARGS__) : \
        hcnse_log_stderr(err, __VA_ARGS__))

#else
#define hcnse_log_error1(level, err, ...) \
    (hcnse_log_get_global() ? \
        hcnse_log_error(level, hcnse_log_get_global(), err, __VA_ARGS__) : 0)

#define hcnse_log_debug1(err, ...) \
    (hcnse_log_get_global() ? \
        hcnse_log_debug(hcnse_log_get_global(), err, __VA_ARGS__) : 0)
#endif

#define HCNSE_FILE_LINE  (__FILE__ ":" hcnse_stringify(__LINE__))


extern hcnse_log_t *hcnse_log_global;

void hcnse_log_console(hcnse_fd_t fd, hcnse_err_t err, const char *fmt, ...);
hcnse_log_t *hcnse_log_create(hcnse_conf_t *conf);
hcnse_err_t hcnse_log_create1(hcnse_log_t **in_log, hcnse_conf_t *conf);
hcnse_err_t hcnse_log_update(hcnse_log_t *log, hcnse_conf_t *conf);
void hcnse_log_error(hcnse_uint_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...);
void hcnse_log_destroy(hcnse_log_t *log);

#endif /* INCLUDED_HCNSE_LOG_H */
