#include <stdio.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "files.h"
#include "syslog.h"
#include "process.h"


err_t parse_argv(int argc, char const *argv[])
{

    return OK;
}


int main(int argc, char const *argv[])
{
    err_t err;

    err = parse_argv(argc, argv);
    if (err != OK) {
        write_stderr("Invalid input parameters\n");
        goto failed;
    }


    /* 
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */

    err = daemon_init();
    if (err != OK) {
        log_error(LOG_EMERG, "Failed to initialization daemon process", err);
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
        log_error(LOG_EMERG, "Failed to initialization winsock", err);
        goto failed;
    }
#endif

    while (1);

    return OK;

failed:

#if (SYSTEM_WINDOWS)
    system("pause");
#endif
    return ERR_FAILED;
}
