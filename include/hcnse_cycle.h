#ifndef INCLUDED_HCNSE_CYCLE_H
#define INCLUDED_HCNSE_CYCLE_H

#include "hcnse_portable.h"
#include "hcnse_common.h"


struct hcnse_cycle_s {
    hcnse_conf_t *conf;
    hcnse_log_t *log;
    hcnse_pool_t *pool;

    hcnse_list_t *modules;

    hcnse_file_t *conf_file;


    hcnse_list_t *listeners;
    hcnse_list_t *connections;
    hcnse_list_t *free_connections;
};

struct hcnse_conf_core_s {
    hcnse_flag_t daemon;
    hcnse_int_t priority;
    char *workdir;

    /* ... */
};

typedef struct hcnse_conf_core_s hcnse_conf_core_t;






#if 0
hcnse_err_t hcnse_cycle_update_by_conf(hcnse_cycle_t *cycle,
    hcnse_conf_t *conf);

hcnse_err_t hcnse_cycle_create_inherited(hcnse_cycle_t *cycle);

void hcnse_cycle_single_process(hcnse_cycle_t *cycle);
#endif

#endif /* INCLUDED_HCNSE_CYCLE_H */
