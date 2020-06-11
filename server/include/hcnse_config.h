#ifndef INCLUDED_HCNSE_CONFIG_H
#define INCLUDED_HCNSE_CONFIG_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

struct hcnse_config_s {
    hcnse_pool_t *pool;
    hcnse_file_t *file;

    hcnse_list_t *addr_and_port;
    hcnse_list_t *addr_and_port6;

    char *log_fname;
    size_t log_size;
    bool log_rewrite;
    uint8_t log_level;

    bool daemon;
    char *workdir;
    int8_t priority; // XXX: Signed value
    hcnse_msec_t timer;
    char *user;
    char *group;
    uint32_t worker_processes;
    uint32_t worker_connections;

    bool ssl_on;
    char *ssl_certfile;
    char *ssl_keyfile;
};


hcnse_err_t hcnse_config_create_and_parse(hcnse_conf_t **in_conf,
    const char *fname);
void hcnse_config_destroy(hcnse_conf_t *conf);

#endif /* INCLUDED_HCNSE_CONFIG_H */
