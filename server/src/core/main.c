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
    hcnse_lnode_t *iter;
    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];

    hcnse_fprintf(HCNSE_STDOUT, "conf_file: \"%s\"\n", conf->file->name);
    hcnse_fprintf(HCNSE_STDOUT, "log_file: \"%s\"\n", conf->log_fname);
    hcnse_fprintf(HCNSE_STDOUT, "log_level: %u\n", conf->log_level);
    hcnse_fprintf(HCNSE_STDOUT, "log_size: %zu\n", conf->log_size);
    hcnse_fprintf(HCNSE_STDOUT, "workdir: \"%s\"\n", conf->workdir);
    hcnse_fprintf(HCNSE_STDOUT, "priority: %d\n", conf->priority);

    if (conf->ssl_on) {
        hcnse_fprintf(HCNSE_STDOUT, "ssl: on\n");
        if (conf->ssl_certfile) {
            hcnse_fprintf(HCNSE_STDOUT,
                "ssl_certfile: \"%s\"\n", conf->ssl_certfile);
        }
        else {
            hcnse_fprintf(HCNSE_STDOUT,
                "WARNING: SSL certificate file is undefined\n");
        }
        if (conf->ssl_keyfile) {
            hcnse_fprintf(HCNSE_STDOUT,
                "ssl_keyfile: \"%s\"\n", conf->ssl_keyfile);
        }
        else {
            hcnse_fprintf(HCNSE_STDOUT,
                "WARNING: SSL key file is undefined\n");
        }
    }
    else {
        hcnse_fprintf(HCNSE_STDOUT, "ssl: off\nWARNING: SSL is disable\n");
    }

    for (iter = hcnse_list_first(conf->listeners);
                                        iter; iter = hcnse_list_next(iter)) {
        listener = hcnse_list_cast_ptr(hcnse_listener_t, iter);
        hcnse_fprintf(HCNSE_STDOUT, "listen: %s:%s\n",
            hcnse_listener_get_addr(str_ip, &(listener->sockaddr)),
            hcnse_listener_get_port(str_port, &(listener->sockaddr)));
    }
}

static void hcnse_show_version_info(void)
{
    hcnse_fprintf(HCNSE_STDOUT, "%s (%s)\n",
                        hcnse_version_info(), hcnse_build_time());
#ifdef HCNSE_COMPILER
    hcnse_fprintf(HCNSE_STDOUT, "Built by %s\n", HCNSE_COMPILER);
#endif
}


int main(int argc, char *const argv[])
{
    hcnse_listener_t *listener;
    hcnse_conf_t *conf;
    hcnse_lnode_t *iter;
    hcnse_err_t err;

    err = hcnse_parse_argv(argc, argv);
    if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s\n", "Invalid input parameters");
        goto failed;
    }

    if (show_version) {
        hcnse_show_version_info();
        return 0;
    }
    if (show_help) {
        hcnse_fprintf(HCNSE_STDOUT, "%s\n", "Some help info");
        return 0;
    }

#if (HCNSE_WIN32)
    err = hcnse_winsock_init_v22();
    if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s: %s\n",
            "Failed to initialize Winsock 2.2", hcnse_strerror(err));
        goto failed;
    }
#endif

    err = hcnse_config_init(&conf, conf_file);
    if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s: %s\n",
            "Failed to initialize config", hcnse_strerror(err));
        goto failed;
    }

    if (test_config) {
        hcnse_show_config_info(conf);
        hcnse_config_fini(conf);
        return 0;
    }

    err = hcnse_process_set_workdir(conf->workdir);
    if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s: %s\n",
            "Failed to set workdir", hcnse_strerror(err));
        goto failed;
    }

    for (iter = hcnse_list_first(conf->listeners);
                                        iter; iter = hcnse_list_next(iter)) {
        listener = hcnse_list_cast_ptr(hcnse_listener_t, iter);
        err = hcnse_listener_start_listen(listener);
        if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s: %s\n",
            "Failed to create listening socket", hcnse_strerror(err));
        goto failed;
        }
    }

    err = hcnse_log_init(&(conf->log), conf);
    if (err != HCNSE_OK) {
        hcnse_fprintf(HCNSE_STDERR, "%s: %s\n",
            "Failed to set open log", hcnse_strerror(err));
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

    hcnse_single_process_cycle(conf);

    return 0;

failed:
#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}
