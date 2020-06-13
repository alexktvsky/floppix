#ifndef INCLUDED_HCNSE_CORE_H
#define INCLUDED_HCNSE_CORE_H

typedef int                            hcnse_err_t;
typedef struct hcnse_conf_s            hcnse_conf_t;
typedef struct hcnse_conf_directive_s  hcnse_conf_directive_t;
typedef struct hcnse_cycle_s           hcnse_cycle_t;
typedef struct hcnse_log_s             hcnse_log_t;
typedef struct hcnse_listener_s        hcnse_listener_t;
typedef struct hcnse_connect_s         hcnse_connect_t;
typedef struct hcnse_list_s            hcnse_list_t;
typedef struct hcnse_list_node_s       hcnse_list_node_t;
typedef struct hcnse_pool_s            hcnse_pool_t;
typedef struct hcnse_module_s          hcnse_module_t;
typedef struct hcnse_event_s           hcnse_event_t;
typedef struct hcnse_event_actions_s   hcnse_event_actions_t;
typedef struct hcnse_regex_compile_s   hcnse_regex_compile_t;


typedef void (*hcnse_conf_dir_handler_t)(int argc, char **argv);
typedef hcnse_thread_value_t (*hcnse_thread_function_t)(void *);
typedef void (*hcnse_cleanup_handler_t)(void *data);
typedef void (*hcnse_event_handler_t)(hcnse_event_t *event);


#include "hcnse_errno.h"
#include "hcnse_time.h"
#include "hcnse_memory.h"
#include "hcnse_limits.h"
#include "hcnse_socket.h"
#include "hcnse_sockopt.h"
#include "hcnse_file.h"
#include "hcnse_thread.h"
#include "hcnse_list.h"
#include "hcnse_rbtree.h"
#include "hcnse_config.h"
#include "hcnse_log.h"
#include "hcnse_connection.h"
#include "hcnse_send.h"
#include "hcnse_cycle.h"
#include "hcnse_event.h"
#include "hcnse_string.h"
#include "hcnse_pool.h"
#include "hcnse_process.h"
#include "hcnse_release.h"
#include "hcnse_signal.h"
#include "hcnse_mutex.h"
#include "hcnse_semaphore.h"
#include "hcnse_cpuinfo.h"
#include "hcnse_dso.h"
#include "hcnse_module.h"

// #include "hcnse_regex.h"

#endif /* INCLUDED_HCNSE_CORE_H */
