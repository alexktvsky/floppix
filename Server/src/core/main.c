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
#include "files.h"
#include "syslog.h"
#include "config.h"
#include "process.h"
#include "cycle.h"


static const char *conf_file;


err_t parse_argv(int argc, char const *argv[])
{
    if (argc == 2) {
        conf_file = argv[1];
    }
    else {
        conf_file = "server.conf";
    }
    return OK;
}


int main(int argc, char const *argv[])
{
    config_t *conf;
    err_t err;

#if (SYSTEM_WINDOWS)
    err = winsock_init_v22();
    if (err != OK) {
        fprintf(stderr, "%s\n", "Failed to initialize Winsock 2.2");
        goto error0;
    }
#endif

    err = parse_argv(argc, argv);
    if (err != OK) {
        fprintf(stderr, "%s\n", "Invalid input parameters");
        goto error0;
    }

    err = config_init(&conf, conf_file);
    if (err != OK) {
        fprintf(stderr, "%s\n", get_strerror(err));
        goto error0;
    }

    err = process_set_workdir(conf->workdir);
    if (err != OK) {
        fprintf(stderr, "%s\n", get_strerror(err));
        goto error1;
    }

    for (listener_t *ls = (listener_t *) list_first(conf->listeners);
                                    ls; ls = (listener_t *) list_next(ls)) {
        err = listener_start_listen(ls);
        if (err != OK) {
            fprintf(stderr, "%s\n", get_strerror(err));
            goto error1;
        }
    }



    // err = log_init(conf->logfile, conf->log);
    // if (err != OK) {
    //     fprintf(stderr, "%s\n", get_strerror(err));
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
