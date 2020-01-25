#include <stdio.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "process.h"
#include "sockets.h"
#include "listen.h"
#include "config.h"
#include "syslog.h"


err_t parse_argv(int argc, char const *argv[])
{

    return OK;
}


int main(int argc, char const *argv[])
{
    err_t err;

    err = parse_argv(argc, argv);
    if (err != OK) {
        fprintf(stderr, "%s\n", "Invalid input parameters");
        goto failed;
    }

    err = init_config();
    if (err != OK) {
        fprintf(stderr, "%s\n", "Failed to load config");
        goto failed;
    }

    err = parse_config();
    if (err != OK) {
        fprintf(stderr, "%s\n", "Failed to read config");
        goto failed;
    }

    err = init_log(config_get_logfile(), config_get_maxlog(), LOG_INFO);
    if (err != OK) {
        fprintf(stderr, "%s\n", "Failed to initialization log");
        goto failed;
    }

    /* 
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */

    err = init_daemon();
    if (err != OK) {
        log_error(LOG_EMERG, "Failed to initialization daemon process.", err);
        goto failed;
    }

    /* TODO:
     * regex_init
     * init_signals
     * ssl_init
     * create_mempool
     * init_cycle
     * 
     * single_process_cycle(cycle);
     * master_process_cycle(cycle);
     * 
     */

#if (SYSTEM_WINDOWS)
    if (init_winsock() != OK) {
        log_error(LOG_EMERG, "Failed to initialization winsock.", err);
        goto failed;
    }
#endif

    while (1);

    return OK;

failed:
    fini_log();
    fini_config();
#if (SYSTEM_WINDOWS)
    system("pause");
#endif
    return ERR_FAILED;
}
