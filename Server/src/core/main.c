#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "errors.h"
#include "daemon.h"


//#include <pthread.h>

#include "sockets.h"
#include "listen.h"
#include "pools.h"
#include "config.h"
#include "core.h"
#include "syslog.h"

#if (SYSTEM_LINUX)
#include <sys/sysinfo.h>
#endif 


#ifdef DEBUG
typedef struct {
    /* add IPv4 */
    struct sockaddr_in6 IP6SockClient;
    unsigned size_IP6SockClient;
    char ip[INET6_ADDRSTRLEN];
    socket_t socket;
    int port;
    int host_port;
} connect_unit_t;
#endif



void parse_argv(int argc, char *argv[])
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


int main(int argc, char *argv[])
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
     * here we close TTY, fork off the parent process and run daemon.
     */

    err = init_daemon();
    if (err != XXX_OK) {
        log_and_abort(LOG_EMERG, "Failed to initialization daemon process.", err);
    }

    log_msg(LOG_INFO, "Hello from server!");


#if SYSTEM_WIN32 || SYSTEM_WIN64
    /* Initialization of Windows sockets library */
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


    while(1);


#ifdef DEBUG

    /* Pull connection requests from the queue */
    listen_unit_t *temp = config_get_listeners();
    connect_unit_t scunit1;
    scunit1.size_IP6SockClient = sizeof(scunit1.IP6SockClient);
    scunit1.socket = accept(temp->socket, 
                           (struct sockaddr *) &scunit1.IP6SockClient,
                            &scunit1.size_IP6SockClient);

    inet_ntop(AF_INET6, &(scunit1.IP6SockClient.sin6_addr),
              scunit1.ip, INET6_ADDRSTRLEN);
    scunit1.port = ntohs(scunit1.IP6SockClient.sin6_port);
    scunit1.host_port = temp->port;


    char buf[1024];
    int bytes_read;

    while (1) {
        bytes_read = recv(scunit1.socket, buf, 1024, 0);
        if (bytes_read <= 0) {
            break;
        }
        buf[bytes_read] = '\0';
        fprintf(stdout, "Client: '%s'\n", buf);
    }
#endif



    fini_config();
    fini_log();

    //close_socket((config->listeners)[2].socket);
    //close_socket(scunit1.socket);

    exit(0);
}
