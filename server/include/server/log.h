#ifndef INCLUDED_LOG_H
#define INCLUDED_LOG_H

#include <stdint.h>
#include <stddef.h>

#include "server/errors.h"
#include "os/files.h"


#define HCNSE_LOG_EMERG   0
#define HCNSE_LOG_ERROR   1
#define HCNSE_LOG_WARN    2
#define HCNSE_LOG_INFO    3
#define HCNSE_LOG_DEBUG   4

#define hcnse_log_debug(level, log, ...) \
    hcnse_log_msg(level, log, __VA_ARGS__)

#define hcnse_log_error1(log, err, ...) \
    hcnse_log_error(HCNSE_LOG_ERROR, log, err, __VA_ARGS__)

typedef struct hcnse_log_s hcnse_log_t;

hcnse_err_t hcnse_log_init(hcnse_log_t **in_log, const char *fname,
    uint8_t level);
void hcnse_log_fini(hcnse_log_t *log);

void hcnse_log_msg(uint8_t level, hcnse_log_t *log, const char *fmt, ...);
void hcnse_log_error(uint8_t level, hcnse_log_t *log, hcnse_err_t err,
    const char *fmt, ...);

#endif /* INCLUDED_LOG_H */
