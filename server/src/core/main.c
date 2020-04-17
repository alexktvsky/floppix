#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "syshead.h"
#include "connection.h"
#include "sys_errno.h"
#include "sys_process.h"
#include "errors.h"
#include "log.h"
#include "config.h"
#include "cycle.h"
#include "release.h"


static char *conf_file = "server.conf";
static bool show_version;
static bool show_help;
static bool test_config;
static bool quiet_mode;


err_t parse_argv(int argc, char *const argv[])
{
    for (int i = 1; i < argc; i++) {
        char *p = argv[i];
        if (*p++ != '-') {
            return ERR_FAILED;
        }
        while (*p) {
            switch (*p++) {
            case 'v':
                show_version = true;
                break;

            case 'V':
                show_version = true;
                break;

            case 'h':
                show_help = true;
                break;

            case 'H':
                show_help = true;
                break;

            case 't':
                test_config = true;
                break;

            case 'T':
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
                    return ERR_FAILED;
                }
                break;

            case 'C':
                if (argv[i++]) {
                    conf_file = argv[i];
                }
                else {
                    return ERR_FAILED;
                }
                break;

            /* smth else */
            default:
                return ERR_FAILED;
            }
        }
    }
    return OK;
}

void show_config_info(config_t *conf)
{
    fprintf(stdout, "conf_file: \"%s\"\n", conf->file->name);
    fprintf(stdout, "log_file: \"%s\"\n", conf->log_file);
    fprintf(stdout, "log_level: %u\n", conf->log_level);
    fprintf(stdout, "log_size: %zu\n", conf->log_size);
    fprintf(stdout, "workdir: \"%s\"\n", conf->workdir);
    fprintf(stdout, "priority: %d\n", conf->priority);

    if (conf->ssl_on) {
        fprintf(stdout, "ssl: on\n");
        if (conf->ssl_certfile) {
            fprintf(stdout, "ssl_certfile: \"%s\"\n", conf->ssl_certfile);
        }
        else {
            fprintf(stdout, "WARNING: SSL certificate file is undefined\n");
        }
        if (conf->ssl_keyfile) {
            fprintf(stdout, "ssl_keyfile: \"%s\"\n", conf->ssl_keyfile);
        }
        else {
            fprintf(stdout, "WARNING: SSL key file is undefined\n");
        }
    }
    else {
        fprintf(stdout, "ssl: off\nWARNING: SSL is disable\n");
    }

    listnode_t *iter;
    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];
    
    for (iter = list_first(conf->listeners); iter; iter = list_next(iter)) {
        fprintf(stdout, "listen: %s:%s\n",
            listener_get_addr(str_ip, &(list_cast_ptr(listener_t, iter))->sockaddr),
            listener_get_port(str_port, &(list_cast_ptr(listener_t, iter))->sockaddr));
    }
}


int main(int argc, char *const argv[])
{
    config_t *conf;
    err_t err;

    err = parse_argv(argc, argv);
    if (err != OK) {
        fprintf(stderr, "%s\n", "Invalid input parameters");
        goto error0;
    }

    if (show_version) {
        fprintf(stdout, "%s\n", get_version_info());
        return 0;
    }
    if (show_help) {
        fprintf(stdout, "%s\n", "Some help info");
        return 0;
    }


#if (SYSTEM_WINDOWS)
    err = winsock_init_v22();
    if (err != OK) {
        fprintf(stderr, "%s: %s\n", "Failed to initialize Winsock 2.2",
                                    sys_strerror(sys_errno));
        goto error0;
    }
#endif

    err = config_init(&conf, conf_file);
    if (err != OK) {
        fprintf(stderr, "%s: %s\n", err_strerror(err),
                                    sys_strerror(sys_errno));
        goto error0;
    }

    if (test_config) {
        show_config_info(conf);
        config_fini(conf);
        return 0;
    }

    err = process_set_workdir(conf->workdir);
    if (err != OK) {
        fprintf(stderr, "%s: %s\n", err_strerror(err),
                                    sys_strerror(sys_errno));
        goto error1;
    }

    listnode_t *iter;
    for (iter = list_first(conf->listeners); iter; iter = list_next(iter)) {

        err = listener_start_listen(list_cast_ptr(listener_t, iter));
        if (err != OK) {
        fprintf(stderr, "%s: %s\n", err_strerror(err),
                                    sys_strerror(sys_errno));
            goto error1;
        }
    }



    // err = log_init(conf->logfile, conf->log);
    // if (err != OK) {
    //     fprintf(stderr, "%s\n", err_strerror(err));
    //     goto log_init_failed;
    // }

    /* Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon */
    // err = process_daemon_init();
    // if (err != OK) {
    //     log_error(LOG_EMERG, "Failed to initialization daemon process", err);
    //     goto daemon_init_failed;
    // }

    /* TODO:
     * signals_init
     * ssl_init
     * create_mempool
     * 
     * master_process_cycle(conf);
     * 
     */

    single_process_cycle(conf);

    return 0;


error1:
    config_fini(conf);
error0:
    return 1;
}
