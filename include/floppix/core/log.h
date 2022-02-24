#ifndef FLOPPIX_CORE_LOG_H
#define FLOPPIX_CORE_LOG_H

#include "floppix/system/errno.h"
#include "floppix/system/filesys.h"

#define FPX_LOG_EMERG                 0
#define FPX_LOG_ERROR                 1
#define FPX_LOG_WARN                  2
#define FPX_LOG_INFO                  3
#define FPX_LOG_DEBUG                 4
#define FPX_LOG_INVALID_LEVEL         5

#define fpx_logger_set_global(logger) (fpx_logger_global = logger)
#define fpx_logger_get_global()       (fpx_logger_global)

#define fpx_log_stdout(err, ...)      fpx_log_console(FPX_STDOUT, err, __VA_ARGS__)

#define fpx_log_stderr(err, ...)      fpx_log_console(FPX_STDERR, err, __VA_ARGS__)

#define fpx_log_debug(logger, err, ...)                                        \
    fpx_log_error(FPX_LOG_DEBUG, logger, err, __VA_ARGS__)

#define fpx_log_error1(level, err, ...)                                        \
    (fpx_logger_get_global()                                                   \
            ? fpx_log_error(level, fpx_logger_get_global(), err, __VA_ARGS__)  \
            : fpx_log_stderr(err, __VA_ARGS__))

#if (FPX_DEBUG)
#define fpx_log_debug1(err, ...)                                               \
    (fpx_logger_get_global()                                                   \
            ? fpx_log_debug(fpx_logger_get_global(), err, __VA_ARGS__)         \
            : fpx_log_stdout(err, __VA_ARGS__))
#else
#define fpx_log_debug1(err, ...)                                               \
    if (fpx_logger_get_global()) {                                             \
        fpx_log_debug(fpx_logger_get_global(), err, __VA_ARGS__);              \
    }
#endif

#define FPX_FILE_LINE (__FILE__ ":" fpx_stringify(__LINE__))

#define fpx_assert(exp)                                                        \
    {                                                                          \
        if (fpx_logger_get_global()) {                                         \
            if (!(exp)) {                                                      \
                fpx_log_error(FPX_LOG_EMERG, fpx_logger_get_global(), FPX_OK,  \
                    "%s: Assertion \"%s\" failed", FPX_FILE_LINE,              \
                    fpx_stringify(exp));                                       \
                abort();                                                       \
            }                                                                  \
        }                                                                      \
        else {                                                                 \
            if (!(exp)) {                                                      \
                fpx_log_stderr(FPX_OK, "%s: Assertion \"%s\" failed",          \
                    FPX_FILE_LINE, fpx_stringify(exp));                        \
                abort();                                                       \
            }                                                                  \
        }                                                                      \
    }

typedef struct fpx_log_s fpx_log_t;
typedef struct fpx_logger_s fpx_logger_t;

extern fpx_logger_t *fpx_logger_global;
extern const char *fpx_log_prio[];

void fpx_log_console(fpx_fd_t fd, fpx_err_t err, const char *fmt, ...);
void fpx_log_error(uint level, fpx_logger_t *logger, fpx_err_t err,
    const char *fmt, ...);

fpx_err_t fpx_logger_init(fpx_logger_t **logger);
fpx_err_t fpx_logger_start(fpx_logger_t *logger);
fpx_err_t fpx_logger_add_log_file(fpx_logger_t *logger, uint level,
    const char *fname, size_t size);
fpx_err_t fpx_logger_add_log_fd(fpx_logger_t *logger, uint level, fpx_fd_t fd);

void fpx_logger_destroy(fpx_logger_t *logger);

#endif /* FLOPPIX_CORE_LOG_H */
