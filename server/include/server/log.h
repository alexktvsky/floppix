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


typedef struct hcnse_log_s hcnse_log_t;

struct hcnse_log_s {
    hcnse_file_t *file;
    uint8_t level;
};


// void server_log_error(int level, const char *description, hcnse_err_t errcode);


#endif /* INCLUDED_LOG_H */
