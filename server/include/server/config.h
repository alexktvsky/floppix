#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "server/connection.h"
#include "server/errors.h"
#include "server/mempool.h"
#include "server/list.h"
#include "server/log.h"
#include "os/files.h"

typedef struct hcnse_config_s hcnse_conf_t;


struct hcnse_config_s {
    hcnse_file_t *file;
    char *data;
    hcnse_list_t *listeners;
    hcnse_list_t *free_connects; // Linked list with free and available connections
    char *workdir;
    hcnse_log_t *log;
    char *log_file;
    uint8_t log_level;
    size_t log_size;
    bool ssl_on;
    char *ssl_certfile;
    char *ssl_keyfile;
    int8_t priority; // XXX: Signed value
};


hcnse_err_t hcnse_config_init(hcnse_conf_t **conf, const char *fname);
void hcnse_config_fini(hcnse_conf_t *conf);

#endif /* INCLUDED_CONFIG_H */
