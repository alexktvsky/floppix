#ifndef INCLUDED_HCNSE_LOG_H
#define INCLUDED_HCNSE_LOG_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LOG_EMERG   0
#define HCNSE_LOG_ERROR   1
#define HCNSE_LOG_WARN    2
#define HCNSE_LOG_INFO    3
#define HCNSE_LOG_DEBUG   4

#if (HCNSE_DEBUG)
#define hcnse_log_debug(level, log, ...) \
    hcnse_log_msg(level, log, __VA_ARGS__)
#else
#define hcnse_log_debug(level, log, ...)
#endif

hcnse_err_t hcnse_log_init(hcnse_log_t **in_log, const char *fname,
    uint8_t level, size_t size);

void hcnse_log_fini(hcnse_log_t *log);

void hcnse_log_msg(uint8_t level, hcnse_log_t *log, const char *fmt, ...);

/* For server defined errors */
void hcnse_log_error(uint8_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...);

#endif /* INCLUDED_HCNSE_LOG_H */
