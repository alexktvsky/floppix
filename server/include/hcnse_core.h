#ifndef INCLUDED_HCNSE_CORE_H
#define INCLUDED_HCNSE_CORE_H

typedef int                            hcnse_err_t;
typedef struct hcnse_config_s          hcnse_conf_t;
typedef struct hcnse_log_s             hcnse_log_t;
typedef struct hcnse_listener_s        hcnse_listener_t;
typedef struct hcnse_connect_s         hcnse_connect_t;
typedef struct hcnse_list_s            hcnse_list_t;
typedef struct hcnse_lnode_s           hcnse_lnode_t;
typedef struct hcnse_mempool_s         hcnse_mempool_t;

#include "hcnse_config.h"
#include "hcnse_errno.h"
#include "hcnse_error.h"
#include "hcnse_time.h"
#include "hcnse_socket.h"
#include "hcnse_file.h"
#include "hcnse_thread.h"
#include "hcnse_list.h"
#include "hcnse_log.h"
#include "hcnse_connection.h"
#include "hcnse_cycle.h"
#include "hcnse_event.h"
#include "hcnse_memory.h"
#include "hcnse_string.h"
#include "hcnse_mempool.h"
#include "hcnse_process.h"
#include "hcnse_release.h"
#include "hcnse_signal.h"
#include "hcnse_mutex.h"
#include "hcnse_semaphore.h"

#endif /* INCLUDED_HCNSE_CORE_H */
