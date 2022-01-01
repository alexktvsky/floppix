#ifndef FPX_CORE_SERVER_H
#define FPX_CORE_SERVER_H

#include "fpx.core.forwards.h"
#include "fpx.system.errno.h"
#include "fpx.system.pool.h"
#include "fpx.algo.list.h"
#include "fpx.core.log.h"
#include "fpx.core.command.h"
#include "fpx.core.module.h"
#include "fpx.core.listen.h"
#include "fpx.core.parse.h"









/*
typedef struct fpx_event_s           fpx_event_t;
*/ 



/*
 * A structure to store information of cintext for each virtual server
 */
struct fpx_server_s {
    /* Number of command line arguments passed to the program */
    fpx_int_t argc;
    /* The command line arguments */
    const char *const *argv;

    fpx_pool_t *pool;
    fpx_logger_t *logger;
    fpx_list_t *modules;

    fpx_config_t *config;

    fpx_list_t *listeners;
    fpx_list_t *connections;
    fpx_list_t *free_connections;

    fpx_bool_t daemon;
    fpx_int_t priority;

    char *workdir;
    char *user;
    char *group;


};




/*

void fpx_server_single_process(fpx_server_t *server);
void fpx_server_master_process(fpx_server_t *server);

 */


#endif /* FPX_CORE_SERVER_H */
