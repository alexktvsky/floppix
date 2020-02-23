#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "sys_files.h"
#include "sys_errno.h"
#include "log.h"
#include "config.h"
#include "sys_process.h"
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
    fprintf(stdout, "conffile: \"%s\"\n", conf->file->name);
    fprintf(stdout, "logfile: \"%s\"\n", conf->logfile);
    fprintf(stdout, "loglevel: %u\n", conf->loglevel);
    fprintf(stdout, "logsize: %zu\n", conf->logsize);
    fprintf(stdout, "workdir: \"%s\"\n", conf->workdir);
    fprintf(stdout, "priority: %d\n", conf->priority);

    if (conf->use_ssl) {
        fprintf(stdout, "ssl: on\n");
        fprintf(stdout, "ssl_certificate: \"%s\"\n", conf->cert);
        fprintf(stdout, "ssl_certificate_key: \"%s\"\n", conf->cert_key);
    }
    else {
        fprintf(stdout, "ssl: off\nWARNING: SSL is disable\n");
    }

    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];
    for (listener_t *ls = (listener_t *) list_first(conf->listeners);
                                    ls; ls = (listener_t *) list_next(ls)) {
        fprintf(stdout, "listen: %s:%s\n",
            get_addr(str_ip, &(ls)->sockaddr),
            get_port(str_port, &(ls)->sockaddr));
    }
    return;
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

    for (listener_t *ls = (listener_t *) list_first(conf->listeners);
                                    ls; ls = (listener_t *) list_next(ls)) {
        err = listener_start_listen(ls);
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
