#include "hcnse_portable.h"
#include "hcnse_core.h"


static char *conf_file = "hcnse.conf";
static hcnse_uint_t show_version;
static hcnse_uint_t show_help;
static hcnse_uint_t test_conf;
static hcnse_uint_t quiet_mode;


static hcnse_err_t
hcnse_parse_argv(int argc, char *const argv[])
{
    for (int i = 1; i < argc; i++) {
        char *p = argv[i];
        if (*p++ != '-') {
            return HCNSE_FAILED;
        }
        while (*p) {
            switch (*p++) {
            case 'v':
                show_version = 1;
                break;

            case 'h':
                show_help = 1;
                break;

            case 't':
                test_conf = 1;
                break;

            case 'q':
                quiet_mode = 1;
                break;

            case 'c':
                if (argv[i++]) {
                    conf_file = argv[i];
                }
                else {
                    return HCNSE_FAILED;
                }
                break;

            /* smth else */
            default:
                return HCNSE_FAILED;
            }
        }
    }
    return HCNSE_OK;
}

static void
hcnse_show_version_info(void)
{
    hcnse_log_stdout(HCNSE_OK, "%s (%s)", HCNSE_VERSION_INFO, HCNSE_BUILD_TIME);
    hcnse_log_stdout(HCNSE_OK, "Target system: %s", HCNSE_SYSTEM_NAME);
#ifdef HCNSE_COMPILER
    hcnse_log_stdout(HCNSE_OK, "Built by %s", HCNSE_COMPILER);
#endif
}

static void
hcnse_show_help_info(void)
{
    hcnse_log_stdout(HCNSE_OK, "%s", "Some help info");
}

int
main(int argc, char *const argv[])
{
    hcnse_cycle_t *cycle;
    hcnse_pool_t *pool;
    hcnse_conf_t *conf;
    hcnse_log_t *log;

    hcnse_list_t *listeners;
    hcnse_list_t *connections;
    hcnse_list_t *free_connections;

    hcnse_err_t err;


    err = hcnse_parse_argv(argc, argv);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(0, "Invalid input parameters");
        goto failed;
    }

    if (show_version) {
        hcnse_show_version_info();
        return 0;
    }
    if (show_help) {
        hcnse_show_help_info();
        return 0;
    }

#if (HCNSE_WIN32)
    err = hcnse_winsock_init_v22();
    if (err != HCNSE_OK) {
        hcnse_log_stderr(0, "Failed to initialize Winsock 2.2");
        goto failed;
    }
#endif

    pool = hcnse_pool_create(0, NULL);
    if (!pool) {
        err = hcnse_get_errno();
        hcnse_log_stderr(err, "Failed to create pool");
        goto failed;
    }

    cycle = hcnse_pcalloc(pool, sizeof(hcnse_cycle_t));
    if (!cycle) {
        err = hcnse_get_errno();
        hcnse_log_stderr(err, "Failed to create server cycle");
        goto failed;
    }

    /* Init linked lists of resources */
    listeners = hcnse_list_create(pool);
    if (!listeners) {
        err = hcnse_get_errno();
        hcnse_log_stderr(err, "Failed to create linked list");
        goto failed;
    }

    connections = hcnse_list_create(pool);
    if (!connections) {
        err = hcnse_get_errno();
        hcnse_log_stderr(err, "Failed to create linked list");
        goto failed;
    }

    free_connections = hcnse_list_create(pool);
    if (!free_connections) {
        err = hcnse_get_errno();
        hcnse_log_stderr(err, "Failed to create linked list");
        goto failed;
    }

    cycle->pool = pool;
    cycle->listeners = listeners;
    cycle->connections = connections;
    cycle->free_connections = free_connections;

    err = hcnse_conf_init_and_parse(&conf, pool, conf_file);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to initialize conf");
        goto failed;
    }

    // if (test_conf) {
    //     hcnse_show_conf_info(conf);
    //     hcnse_conf_destroy(conf);
    //     hcnse_pool_destroy(pool);
    //     return 0;
    // }

    // err = hcnse_cycle_update_by_conf(cycle, conf);
    // if (err != HCNSE_OK) {
    //     hcnse_log_stderr(err, "Failed to update cycle by conf");
    //     goto failed;
    // }

    // iter = hcnse_list_first(conf->addr_and_port);
    // for ( ; iter; iter = iter->next) {
    //     ip = iter->data;
    //     iter = iter->next;
    //     port = iter->data;

    //     listener = hcnse_palloc(pool, sizeof(hcnse_listener_t));
    //     if (!listener) {
    //         err = hcnse_get_errno();
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    //     err = hcnse_listener_init_ipv4(listener, ip, port);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }

    //     hcnse_pool_cleanup_add(pool, listener, hcnse_listener_close);

    //     err = hcnse_listener_open(listener);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    //     err = hcnse_list_push_back(cycle->listeners, listener);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    // }

    // iter = hcnse_list_first(conf->addr_and_port6);
    // for ( ; iter; iter = iter->next) {
    //     ip = iter->data;
    //     iter = iter->next;
    //     port = iter->data;

    //     listener = hcnse_palloc(pool, sizeof(hcnse_listener_t));
    //     if (!listener) {
    //         err = hcnse_get_errno();
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    //     err = hcnse_listener_init_ipv6(listener, ip, port);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }

    //     hcnse_pool_cleanup_add(pool, listener, hcnse_listener_close);

    //     err = hcnse_listener_open(listener);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    //     err = hcnse_list_push_back(cycle->listeners, listener);
    //     if (err != HCNSE_OK) {
    //         hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
    //         goto failed;
    //     }
    // }

    // err = hcnse_process_set_workdir(conf->workdir);
    // if (err != HCNSE_OK) {
    //     hcnse_log_stderr(err, "Failed to set workdir %s", conf->workdir);
    //     goto failed;
    // }

    // err = hcnse_log_create1(&log, conf);
    // if (err != HCNSE_OK) {
    //     hcnse_log_stderr(err, "%s", "Failed to set open log");
    //     goto failed;
    // }

    // cycle->log = log;
    // hcnse_log_set_global(log);


    // /*
    //  * Now log file is available and server can write error mesages in it, so
    //  * here we close TTY, fork off the parent process and run daemon
    //  */
    // if (conf->daemon) {
    //     err = hcnse_process_daemon_init();
    // }
    // if (err != HCNSE_OK) {
    //     hcnse_log_error(HCNSE_LOG_EMERG, log, err, "Failed to become daemon");
    //     goto failed;
    // }

    // /* 
    //  * TODO:
    //  * hcnse_signals_init
    //  * hcnse_ssl_init
    //  * 
    //  * hcnse_master_process_cycle(conf);
    //  * 
    //  */

    // if (!(conf->ssl_on)) {
    //     hcnse_log_error(HCNSE_LOG_WARN, log, HCNSE_OK, "SSL is disable");
    // }

    // while (1);
    // hcnse_single_process_cycle(conf);

    return 0;

failed:
#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}
