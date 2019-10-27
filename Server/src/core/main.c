#include <stdio.h>

#include "platform.h"
#include "error_proc.h"
#include "daemon.h"



/* move this to init.c */
#include <stdlib.h>
#include <pthread.h>

#include "sockets.h"
#include "listen.h"
#include "pools.h"
#include "config.h"
#include "core.h"
#include "syslog.h"

#if (SYSTEM_LINUX)
#include <sys/sysinfo.h>
#endif 

#define DEFAULT_CONFIG_FILE "server.conf"


typedef struct {
    /* add IPv4 */
    struct sockaddr_in6 IP6SockClient;
    unsigned size_IP6SockClient;
    char ip[INET6_ADDRSTRLEN];
    socket_t socket;
    int port;
    int host_port;
} connect_unit_t;



void parse_argv(int argc, char *argv[])
{
    /* 
     * Parse input parametrs:
     * argv[1] config file
     * argv[1] -v version
     * argv[1] -h help
     */
    if (!argv[1]) {
        set_config_filename(DEFAULT_CONFIG_FILE);
    }
    else {
        set_config_filename(argv[1]);
    }


}


int main(int argc, char *argv[])
{
    status_t err;

    err = init_config();
    if (err != OK) {
        fprintf(stderr, "Init config failed with code %d: %s\n",
                                                err, set_strerror(err));
        abort();
    }

    parse_argv(argc, argv);

    err = parse_config();
    if (err != OK) {
        fprintf(stderr, "Read config failed with code %d: %s\n",
                                                err, set_strerror(err));
        abort();
    }

    err = init_log(config_get_logfile(), config_get_maxlog(), LOG_INFO);
    if (err != OK) {
        fprintf(stderr, "Init log failed with code %d: %s\n",
                                                err, set_strerror(err));
        abort();
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
    if (err != OK) {
        fprintf(stderr, "Init daemon process failed with code %d: %s\n",
                                                err, set_strerror(err));
        abort();
    }

    err = init_listen_sockets(config_get_listeners());
    if (err != OK) {
        abort();
    }
   
    log_msg(LOG_INFO, "Hello from daemon!");


    if (!config_get_nprocs()) {
        /* Platform depends code sction */
        /* get_nprocs(); */
    }





#ifndef DEBUG
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
