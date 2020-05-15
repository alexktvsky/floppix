#include "hcnse_portable.h"
#include "hcnse_core.h"


static char *conf_file = "server.conf";
static bool show_version;
static bool show_help;
static bool test_config;
static bool quiet_mode;


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
                show_version = true;
                break;

            case 'h':
                show_help = true;
                break;

            case 't':
                test_config = true;
                break;

            case 'q':
                quiet_mode = true;
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
hcnse_show_config_info(hcnse_conf_t *conf)
{
    char *ip;
    char *port;
    hcnse_list_node_t *iter;

    hcnse_log_stdout(HCNSE_OK, "conf_file: \"%s\"", conf->file->name);
    hcnse_log_stdout(HCNSE_OK, "log_file: \"%s\"", conf->log_fname);
    hcnse_log_stdout(HCNSE_OK, "log_size: %zu%s",
        hcnse_convert_to_prefix(conf->log_size),
        hcnse_get_metric_prefix(conf->log_size));

    if (conf->log_rewrite) {
        hcnse_log_stdout(HCNSE_OK, "log_rewrite: on"); 
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "log_rewrite: off"); 
    }

    switch (conf->log_level) {
    case HCNSE_LOG_EMERG:
        hcnse_log_stdout(HCNSE_OK, "log_level: emerg");
        break;
    case HCNSE_LOG_ERROR:
        hcnse_log_stdout(HCNSE_OK, "log_level: error");
        break;
    case HCNSE_LOG_WARN:
        hcnse_log_stdout(HCNSE_OK, "log_level: warn");
        break;
    case HCNSE_LOG_INFO:
        hcnse_log_stdout(HCNSE_OK, "log_level: info");
        break;
    case HCNSE_LOG_DEBUG:
        hcnse_log_stdout(HCNSE_OK, "log_level: debug");
        break;
    }
    hcnse_log_stdout(HCNSE_OK, "workdir: \"%s\"", conf->workdir);
    hcnse_log_stdout(HCNSE_OK, "priority: %d", conf->priority);

    hcnse_log_stdout(HCNSE_OK, "timer: %lu", conf->timer);
    if (conf->user) {
        hcnse_log_stdout(HCNSE_OK, "user: \"%s\"", conf->user);
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "WARNING: user is undefined");
    }
    if (conf->group) {
        hcnse_log_stdout(HCNSE_OK, "group: \"%s\"", conf->group);
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "WARNING: group is undefined");
    }

    if (conf->worker_processes) {
        hcnse_log_stdout(HCNSE_OK, "worker_processes: %lu",
            conf->worker_processes);
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "worker_processes: auto");
    }

    if (conf->worker_connections) {
        hcnse_log_stdout(HCNSE_OK, "worker_connections: %lu",
            conf->worker_connections);
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "worker_connections: nolimit");
    }

    if (conf->ssl_on) {
        hcnse_log_stdout(HCNSE_OK, "ssl: on");
        if (conf->ssl_certfile) {
            hcnse_log_stdout(HCNSE_OK,
                "ssl_certfile: \"%s\"", conf->ssl_certfile);
        }
        else {
            hcnse_log_stdout(HCNSE_OK,
                "WARNING: SSL certificate file is undefined");
        }
        if (conf->ssl_keyfile) {
            hcnse_log_stdout(HCNSE_OK,
                "ssl_keyfile: \"%s\"", conf->ssl_keyfile);
        }
        else {
            hcnse_log_stdout(HCNSE_OK,
                "WARNING: SSL key file is undefined");
        }
    }
    else {
        hcnse_log_stdout(HCNSE_OK, "ssl: off\nWARNING: SSL is disable");
    }

    /* Print IPv4 addr and port */
    iter = hcnse_list_first(conf->addr_and_port);
    for ( ; iter; iter = iter->next) {
        ip = iter->data;
        iter = iter->next;
        port = iter->data;
        hcnse_log_stdout(HCNSE_OK, "listen %s:%s", ip, port);
    }

    /* Print IPv6 addr and port */
    iter = hcnse_list_first(conf->addr_and_port6);
    for ( ; iter; iter = iter->next) {
        ip = iter->data;
        iter = iter->next;
        port = iter->data;
        hcnse_log_stdout(HCNSE_OK, "listen %s:%s", ip, port);
    }
}

static void
hcnse_show_version_info(void)
{
    hcnse_log_stdout(HCNSE_OK,
        "%s (%s)", hcnse_version_info(), hcnse_build_time());
#ifdef HCNSE_COMPILER
    hcnse_log_stdout(HCNSE_OK, "Built by %s", HCNSE_COMPILER);
#endif
}

static void hcnse_show_help_info(void)
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

    hcnse_listener_t *listener;
    hcnse_list_node_t *iter;
    char *ip;
    char *port;

    hcnse_err_t err;

    // hcnse_log_set_global()


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

    pool = hcnse_pool_create(NULL);
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

    hcnse_regex_init();

    err = hcnse_config_create_and_parse(&conf, conf_file);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to initialize config");
        goto failed;
    }

    if (test_config) {
        hcnse_show_config_info(conf);
        hcnse_config_destroy(conf);
        hcnse_pool_destroy(pool);
        return 0;
    }

    err = hcnse_cycle_update_by_conf(cycle, conf);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to update cycle by config");
        goto failed;
    }

    iter = hcnse_list_first(conf->addr_and_port);
    for ( ; iter; iter = iter->next) {
        ip = iter->data;
        iter = iter->next;
        port = iter->data;

        listener = hcnse_palloc(pool, sizeof(hcnse_listener_t));
        if (!listener) {
            err = hcnse_get_errno();
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }
        err = hcnse_listener_init_ipv4(listener, ip, port);
        if (err != HCNSE_OK) {
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }

        hcnse_pool_cleanup_register(pool, listener, hcnse_listener_close);

        err = hcnse_listener_open(listener);
        if (err != HCNSE_OK) {
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }
    }

    iter = hcnse_list_first(conf->addr_and_port6);
    for ( ; iter; iter = iter->next) {
        ip = iter->data;
        iter = iter->next;
        port = iter->data;

        listener = hcnse_palloc(pool, sizeof(hcnse_listener_t));
        if (!listener) {
            err = hcnse_get_errno();
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }
        err = hcnse_listener_init_ipv6(listener, ip, port);
        if (err != HCNSE_OK) {
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }

        hcnse_pool_cleanup_register(pool, listener, hcnse_listener_close);

        err = hcnse_listener_open(listener);
        if (err != HCNSE_OK) {
            hcnse_log_stderr(err, "Failed to listen %s:%s", ip, port);
            goto failed;
        }
    }

    err = hcnse_process_set_workdir(conf->workdir);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to set workdir %s", conf->workdir);
        goto failed;
    }

    err = hcnse_log_create1(&log, conf);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "%s", "Failed to set open log");
        goto failed;
    }

    cycle->log = log;
    hcnse_log_set_global(log);


    /* Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon */
#if !(HCNSE_NO_DAEMON)
    err = hcnse_process_daemon_init();
    if (err != HCNSE_OK) {
        hcnse_log_error(HCNSE_LOG_EMERG, log, err, "Failed to become daemon");
        goto failed;
    }
#endif

    /* TODO:
     * hcnse_signals_init
     * hcnse_ssl_init
     * 
     * hcnse_master_process_cycle(conf);
     * 
     */

    if (!(conf->ssl_on)) {
        hcnse_log_error(HCNSE_LOG_WARN, log, HCNSE_OK, "SSL is disable");
    }

    while (1);
    // hcnse_single_process_cycle(conf);

    return 0;

failed:
#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}
