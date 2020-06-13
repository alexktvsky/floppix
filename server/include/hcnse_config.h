#ifndef INCLUDED_HCNSE_CONFIG_H
#define INCLUDED_HCNSE_CONFIG_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_CONF_TAKE0       0x00000001
#define HCNSE_CONF_TAKE1       0x00000002
#define HCNSE_CONF_TAKE2       0x00000004
#define HCNSE_CONF_TAKE3       0x00000008
#define HCNSE_CONF_TAKE4       0x00000010
#define HCNSE_CONF_TAKE5       0x00000020
#define HCNSE_CONF_TAKE6       0x00000040
#define HCNSE_CONF_TAKE7       0x00000080


#define HCNSE_CONF_NULL_DIRECTIVE  {NULL, 0, NULL}



struct hcnse_conf_directive_s {
    char *name;
    size_t takes;
    hcnse_conf_dir_handler_t handler;
};






#define HCNSE_METRIC_PREFIX_EMPTY      ""
#define HCNSE_METRIC_PREFIX_KILO       "k"
#define HCNSE_METRIC_PREFIX_MEGA       "m"
#define HCNSE_METRIC_PREFIX_GIGA       "g"
#define HCNSE_METRIC_PREFIX_TERA       "t"

#define HCNSE_METRIC_MULTIPLIER_KILO   1000
#define HCNSE_METRIC_MULTIPLIER_MEGA   1000000
#define HCNSE_METRIC_MULTIPLIER_GIGA   1000000000
#define HCNSE_METRIC_MULTIPLIER_TERA   1000000000000


/* Move to hcnse_core.h */
typedef struct hcnse_conf_listener_s hcnse_conf_listener_t;
typedef struct hcnse_conf_log_s hcnse_conf_log_t;
typedef struct hcnse_conf_bin_s hcnse_conf_bin_t;

struct hcnse_conf_listener_s {
    uint32_t addr; /* offset to "char *" */
    uint32_t port; /* offset to "char *" */
};

struct hcnse_conf_log_s {
    uint32_t fname;  /* offset to "char *" */
    uint8_t level;
    uint32_t size;
};

struct hcnse_conf_bin_s {
    /* Flags */
    uint32_t daemon : 1;
    uint32_t ssl : 1;

    /* Scalar parameters */
    int32_t priority;
    uint32_t timer;
    uint32_t workdir;
    uint32_t user;
    uint32_t group;
    uint32_t worker_processes;
    uint32_t worker_connections;

    /* Vector parameters */
    uint32_t n_listeners;
    uint32_t listeners; /* offset to "hcnse_conf_listener_t *" */

    uint32_t n_logs;
    uint32_t logs;  /* offset to "hcnse_conf_log_t *" */



    /*
     * Arrays:
     * hcnse_conf_listener_t listeners[n];
     * hcnse_conf_log_t logs[n];
     * ....
     * ....
     *
     *
     * Raw strings:
     * 127.0.0.1\0log_file1.log\0 ...
     * ....
     * ....
     *
     */
};

struct hcnse_conf_s {
    hcnse_pool_t *pool;
    hcnse_file_t *file;
    hcnse_conf_bin_t *conf_bin;

    // hcnse_conf_listener_t *listeners;
    // hcnse_conf_log_t *logs;

    // bool daemon;
    // char *workdir;
    // int32_t priority; // XXX: Signed value
    // hcnse_msec_t timer;
    // char *user;
    // char *group;
    // uint32_t worker_processes;
    // uint32_t worker_connections;

    // bool ssl;
    // char *ssl_certfile;
    // char *ssl_keyfile;
};


hcnse_err_t hcnse_conf_init_and_parse(hcnse_conf_t **in_conf,
    hcnse_pool_t *pool, const char *fname);
void hcnse_conf_fini(hcnse_conf_t *conf);

const char *hcnse_get_metric_prefix(size_t number);
size_t hcnse_convert_to_prefix(size_t number);

uint8_t hcnse_log_level2uint8(const char *level);

#endif /* INCLUDED_HCNSE_CONFIG_H */
