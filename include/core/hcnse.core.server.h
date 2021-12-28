#ifndef HCNSE_CORE_SERVER_H
#define HCNSE_CORE_SERVER_H

#include "hcnse.type.forwards.h"
#include "hcnse.system.errno.h"
#include "hcnse.system.pool.h"
#include "hcnse.container.list.h"
#include "hcnse.core.log.h"
#include "hcnse.core.command.h"
#include "hcnse.core.module.h"
#include "hcnse.core.listen.h"
#include "hcnse.core.parse.h"









/*
typedef struct hcnse_event_s           hcnse_event_t;
*/ 



/*
 * A structure to store information of cintext for each virtual server
 */
struct hcnse_server_s {
    /* Number of command line arguments passed to the program */
    int argc;
    /* The command line arguments */
    const char *const *argv;

    hcnse_pool_t *pool;
    hcnse_logger_t *logger;
    hcnse_list_t *modules;

    hcnse_config_t *config;

    hcnse_list_t *listeners;
    hcnse_list_t *connections;
    hcnse_list_t *free_connections;

    bool daemon;
    hcnse_int_t priority;

    char *workdir;
    char *user;
    char *group;


};




/*

void hcnse_server_single_process(hcnse_server_t *server);
void hcnse_server_master_process(hcnse_server_t *server);

 */


#endif /* HCNSE_CORE_SERVER_H */
