#ifndef INCLUDED_HCNSE_LOG_H
#define INCLUDED_HCNSE_LOG_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LOG_EMERG                      0
#define HCNSE_LOG_ERROR                      1
#define HCNSE_LOG_WARN                       2
#define HCNSE_LOG_INFO                       3
#define HCNSE_LOG_DEBUG                      4
#define HCNSE_LOG_INVALID_LEVEL              5

#define hcnse_logger_set_global(logger)      (hcnse_logger_global = logger)
#define hcnse_logger_get_global()            (hcnse_logger_global)

#define hcnse_log_stdout(err, ...) \
    hcnse_log_console(HCNSE_STDOUT, err, __VA_ARGS__)

#define hcnse_log_stderr(err, ...) \
    hcnse_log_console(HCNSE_STDERR, err, __VA_ARGS__)

#define hcnse_log_debug(logger, err, ...) \
    hcnse_log_error(HCNSE_LOG_DEBUG, logger, err, __VA_ARGS__)

#define hcnse_log_error1(level, err, ...) \
    (hcnse_logger_get_global() ? \
        hcnse_log_error(level, hcnse_logger_get_global(), err, __VA_ARGS__) : \
        hcnse_log_stderr(err, __VA_ARGS__))

#if (HCNSE_LOG_REDIRECT_TO_STDERR)
#define hcnse_log_debug1(err, ...) \
    (hcnse_logger_get_global() ? \
        hcnse_log_debug(hcnse_logger_get_global(), err, __VA_ARGS__) : \
        hcnse_log_stderr(err, __VA_ARGS__))
#else
#define hcnse_log_debug1(err, ...) \
    (hcnse_logger_get_global() ? \
        hcnse_log_debug(hcnse_logger_get_global(), err, __VA_ARGS__) : 0)
#endif

#define HCNSE_FILE_LINE  (__FILE__ ":" hcnse_stringify(__LINE__))


extern hcnse_logger_t *hcnse_logger_global;
extern const char *hcnse_log_prio[];

/*


#define hcnse_assert(exp) ((exp) ? (void) 0 : hcnse_log_assert(#exp, __FILE__, __LINE__))
void hcnse_log_assert(hcnse_logger_t *logger, const char *expr, const char *file, int line);



hcnse_err_t hcnse_logger_add_log(hcnse_logger_t *logger, hcnse_log_t *log);
hcnse_err_t hcnse_logger_remove_log(hcnse_logger_t *logger, hcnse_log_t *log);
hcnse_err_t hcnse_logger_suspend(hcnse_logger_t *logger);
hcnse_err_t hcnse_logger_resume(hcnse_logger_t *logger);

 */


void hcnse_log_console(hcnse_fd_t fd, hcnse_err_t err, const char *fmt, ...);
void hcnse_log_error(hcnse_uint_t level, hcnse_logger_t *logger,
    hcnse_err_t err, const char *fmt, ...);


hcnse_logger_t *hcnse_logger_create(void);
hcnse_err_t hcnse_logger_create1(hcnse_logger_t **logger);
hcnse_err_t hcnse_logger_start(hcnse_logger_t *logger);
hcnse_err_t hcnse_logger_add_log_file(hcnse_logger_t *logger,
    hcnse_uint_t level, const char *fname, size_t size);
hcnse_err_t hcnse_logger_add_log_fd(hcnse_logger_t *logger,
    hcnse_uint_t level, hcnse_fd_t fd);

void hcnse_logger_destroy(hcnse_logger_t *logger);


#endif /* INCLUDED_HCNSE_LOG_H */
