#ifndef INCLUDED_HCNSE_CONFIG_H
#define INCLUDED_HCNSE_CONFIG_H

#include "hcnse_portable.h"
#include "hcnse_common.h"


#define HCNSE_CONF_TAKE0       0x00000001
#define HCNSE_CONF_TAKE1       0x00000002
#define HCNSE_CONF_TAKE2       0x00000004
#define HCNSE_CONF_TAKE3       0x00000008
#define HCNSE_CONF_TAKE4       0x00000010
#define HCNSE_CONF_TAKE5       0x00000020
#define HCNSE_CONF_TAKE6       0x00000040
#define HCNSE_CONF_TAKE7       0x00000080

#define HCNSE_CONF_MAX_TAKES   8

#define HCNSE_CONF_NULL_DIRECTIVE  {NULL, 0, NULL}


#define HCNSE_CONF_UNSET              -1
#define NGX_CONF_UNSET_PTR             NULL
#define NGX_CONF_UNSET_SIZE            ((size_t) -1)
#define NGX_CONF_UNSET_UINT            ((ngx_int_t) -1)
#define NGX_CONF_UNSET_FLAG            ((hcnse_flag_t) -1)



struct hcnse_conf_directive_s {
    char *name;
    hcnse_flag_t takes;
    hcnse_conf_dir_handler_t handler;
};


hcnse_err_t hcnse_process_conf_file(hcnse_cycle_t *cycle, const char *fname);



#if 0

#define HCNSE_METRIC_PREFIX_EMPTY      ""
#define HCNSE_METRIC_PREFIX_KILO       "k"
#define HCNSE_METRIC_PREFIX_MEGA       "m"
#define HCNSE_METRIC_PREFIX_GIGA       "g"
#define HCNSE_METRIC_PREFIX_TERA       "t"

#define HCNSE_METRIC_MULTIPLIER_KILO   1000
#define HCNSE_METRIC_MULTIPLIER_MEGA   1000000
#define HCNSE_METRIC_MULTIPLIER_GIGA   1000000000
#define HCNSE_METRIC_MULTIPLIER_TERA   1000000000000


struct hcnse_conf_s {
    hcnse_pool_t *pool;
    hcnse_file_t *file;

    uint32_t daemon : 1;
    uint32_t ssl : 1;


    char *workdir;
    hcnse_int_t priority;
    hcnse_msec_t timer;
    char *user;
    char *group;
    hcnse_uint_t worker_processes;
    hcnse_uint_t worker_connections;

    char *ssl_certfile;
    char *ssl_keyfile;
};


hcnse_err_t hcnse_conf_init_and_parse(hcnse_conf_t **in_conf,
    hcnse_pool_t *pool, const char *fname);
void hcnse_conf_fini(hcnse_conf_t *conf);

#endif

#endif /* INCLUDED_HCNSE_CONFIG_H */
