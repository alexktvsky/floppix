#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "connection.h"
#include "files.h"
#include "config.h"
#include "process.h"
#include "cycle.h"
#include "syslog.h"


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

    err = parse_argv(argc, argv);
    if (err != OK) {
        fprintf(stderr, "%s\n", "Invalid input parameters");
        goto failed;
    }

    err = config_init(&conf, conf_file);
    if (err != OK) {
        fprintf(stderr, "%s\n", get_strerror(err));
        goto failed;
    }

#if (SYSTEM_WINDOWS)
    err = winsock_init();
    if (err != OK) {
        fprintf(stderr, "%s\n", "Failed to initialize winsock");
        goto failed;
    }
#endif

    err = open_listening_sockets(conf->listeners);
    if (err != OK) {
        fprintf(stderr, "%s\n", get_strerror(err));
        goto failed;
    }

     
    /* Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon */
    // err = daemon_init(conf->workdir);
    // if (err != OK) {
    //     log_error(LOG_EMERG, "Failed to initialization daemon process", err);
    //     goto failed;
    // }

    /* TODO:
     * signals_init
     * ssl_init
     * create_mempool
     * init_cycle
     * 
     * master_process_cycle(conf);
     * 
     */


    single_process_cycle(conf);

    return 0;

failed:
    if (conf->listeners) {
        close_listening_sockets(conf->listeners);
    }
    if (conf) {
        config_fini(conf);
    }
    return 1;
}
