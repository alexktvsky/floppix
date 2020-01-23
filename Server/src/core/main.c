#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "errors.h"
#include "daemon.h"
#include "sockets.h"
#include "listen.h"
#include "config.h"
#include "syslog.h"


void parse_argv(int argc, char const *argv[])
{
    /* 
     * Parse input parametrs:
     * argv[1] config file
     * argv[1] -v version
     * argv[1] -h help
     */
    if (argc > 1) {
        set_config_filename(argv[1]);
    }
    else {
        set_config_filename(DEFAULT_CONFIG_FILE);
    }
}


int main(int argc, char const *argv[])
{
    status_t err;

    err = init_config();
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to load config.", err);
    }

    parse_argv(argc, argv);

    err = parse_config();
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to read config.", err);
    }

    err = init_log(config_get_logfile(), config_get_maxlog(), LOG_INFO);
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to initialization log.", err);
    }

    /* 
     * init_signal_catch
     * do_test_crypto
     * 
     */

    /* 
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */

    err = init_daemon();
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to initialization daemon process.", err);
    }

    log_msg(LOG_INFO, "Hello from server!");


#if (SYSTEM_WINDOWS)
    if (init_winsock() != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to initialization winsock.", err);
    }
#endif

    err = init_listen_sockets(config_get_listeners());
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to listen for connections.", err);
    }
   
    if (!config_get_nprocs()) {
        /* Platform depends code sction */
        /* get_nprocs(); */
    }


    while (1);


    fini_config();
    fini_log();

    // close_socket((config->listeners)[2].socket);
    // close_socket(scunit1.socket);

    exit(0);
}
