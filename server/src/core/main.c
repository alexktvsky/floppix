#include "hcnse_portable.h"
#include "hcnse_core.h"


static char *conf_file = "server.conf";
static bool show_version;
static bool show_help;
static bool test_config;
static bool quiet_mode;


static hcnse_err_t hcnse_parse_argv(int argc, char *const argv[])
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

static void hcnse_show_config_info(hcnse_conf_t *conf)
{
    hcnse_listener_t *listener;
    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];

    hcnse_log_stdout(HCNSE_OK, "conf_file: \"%s\"", conf->file->name);
    hcnse_log_stdout(HCNSE_OK, "log_file: \"%s\"", conf->log_fname);
    hcnse_log_stdout(HCNSE_OK, "log_size: %zu", conf->log_size);

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

    for (listener = hcnse_list_first(conf->listeners);
                            listener; listener = hcnse_list_next(listener)) {
        hcnse_listener_get_addr(str_ip, &(listener->sockaddr));
        hcnse_listener_get_port(str_port, &(listener->sockaddr));
        hcnse_log_stdout(HCNSE_OK, "listen: %s:%s", str_ip, str_port);
    }
}

static void hcnse_show_version_info(void)
{
    hcnse_log_stdout(HCNSE_OK,
        "%s (%s)", hcnse_version_info(), hcnse_build_time());
#ifdef HCNSE_COMPILER
    hcnse_log_stdout(HCNSE_OK, "Built by %s", HCNSE_COMPILER);
#endif
}

static void hcnse_show_help_info(void)
{
    hcnse_log_stdout(HCNSE_OK, "%s\n", "Some help info");
}


int main(int argc, char *const argv[])
{
    hcnse_listener_t *listener;
    hcnse_conf_t *conf;
    hcnse_err_t err;

    err = hcnse_parse_argv(argc, argv);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(HCNSE_OK, "%s", "Invalid input parameters");
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
        hcnse_log_stderr(err, "%s", "Failed to initialize Winsock 2.2");
        goto failed;
    }
#endif

    err = hcnse_config_init(&conf, conf_file);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "%s", "Failed to initialize config");
        goto failed;
    }

    if (test_config) {
        hcnse_show_config_info(conf);
        hcnse_config_fini(conf);
        return 0;
    }

    err = hcnse_process_set_workdir(conf->workdir);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "%s", "Failed to set workdir");
        goto failed;
    }

    for (listener = hcnse_list_first(conf->listeners);
                            listener; listener = hcnse_list_next(listener)) {
        err = hcnse_listener_start_listen(listener);
        if (err != HCNSE_OK) {
            hcnse_log_stderr(err, "%s", "Failed to create listening socket");
        goto failed;
        }
    }

    err = hcnse_log_init(&(conf->log), conf);
    if (err != HCNSE_OK) {
        hcnse_log_stderr(err, "%s", "Failed to set open log");
        goto failed;
    }

    /* Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon */
#if !(HCNSE_NO_DAEMON)
    err = hcnse_process_daemon_init();
    if (err != HCNSE_OK) {
        hcnse_log_error(HCNSE_LOG_EMERG, conf->log, err,
            "Failed to create server process");
        goto failed;
    }
#endif

    /* TODO:
     * hcnse_signals_init
     * hcnse_ssl_init
     * hcnse_create_mempool
     * 
     * hcnse_master_process_cycle(conf);
     * 
     */

    if (!(conf->ssl_on)) {
        hcnse_log_error(HCNSE_LOG_WARN, conf->log, HCNSE_OK, "SSL is disable");
    }

    hcnse_single_process_cycle(conf);

    return 0;

failed:
#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}
