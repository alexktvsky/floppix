#ifndef INCLUDED_HCNSE_CORE_H
#define INCLUDED_HCNSE_CORE_H


typedef struct hcnse_server_s          hcnse_server_t;
typedef struct hcnse_listener_s        hcnse_listener_t;
typedef struct hcnse_connect_s         hcnse_connect_t;

typedef struct hcnse_command_s         hcnse_command_t;
typedef struct hcnse_cmd_params_s      hcnse_cmd_params_t;
typedef struct hcnse_directive_s       hcnse_directive_t;
typedef struct hcnse_config_s          hcnse_config_t;

typedef struct hcnse_log_s             hcnse_log_t;
typedef struct hcnse_logger_s          hcnse_logger_t;

typedef struct hcnse_array_s           hcnse_array_t;
typedef struct hcnse_list_s            hcnse_list_t;
typedef struct hcnse_list_node_s       hcnse_list_node_t;
typedef struct hcnse_pool_s            hcnse_pool_t;
typedef struct hcnse_module_s          hcnse_module_t;

/*
typedef struct hcnse_event_s           hcnse_event_t;
*/ 

typedef hcnse_err_t (*hcnse_cmd_handler_t)(hcnse_cmd_params_t *params,
    void *data, int argc, char **argv);
typedef hcnse_thread_value_t (*hcnse_thread_function_t)(void *);
typedef void (*hcnse_cleanup_handler_t)(void *data);


#include "hcnse_pool.h"
#include "hcnse_list.h"
#include "hcnse_array.h"
#include "hcnse_config.h"
#include "hcnse_parse.h"
#include "hcnse_log.h"
#include "hcnse_sockaddr.h"
#include "hcnse_listen.h"
#include "hcnse_connection.h"
#include "hcnse_send.h"
#include "hcnse_string.h"
#include "hcnse_release.h"
#include "hcnse_module.h"

#if 0
#include "hcnse_rbtree.h"
#include "hcnse_event.h"
#endif


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

    hcnse_flag_t daemon;
    hcnse_int_t priority;

    char *workdir;
    char *user;
    char *group;


};




/*

void hcnse_server_single_process(hcnse_server_t *server);
void hcnse_server_master_process(hcnse_server_t *server);

 */


#endif /* INCLUDED_HCNSE_CORE_H */
