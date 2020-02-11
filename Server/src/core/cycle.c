#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "syshead.h"

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#endif

#include "errors.h"
#include "mempool.h"
#include "connection.h"
#include "files.h"
#include "syslog.h"
#include "config.h"
#include "cycle.h"


void single_process_cycle(config_t *conf)
{
    struct timeval timeout;
    fd_set rfds;
    fd_set wfds;
    listener_t *lisr;
    connect_t *conn;
    int flag;
    socket_t fdmax;
    err_t err;

    while (1) {

        fdmax = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        /* Add listening sockets to read array */
        for (lisr = conf->listeners; lisr; lisr = lisr->next) {
            FD_SET(lisr->fd, &rfds);
            if (fdmax < lisr->fd) {
                fdmax = lisr->fd;
            }
        }
        /* Add clients sockets to read array */
        for (lisr = conf->listeners; lisr; lisr = lisr->next) {
            for (conn = lisr->connects; conn; conn = conn->next) {
                FD_SET(conn->fd, &rfds);
                if (fdmax < conn->fd) {
                    fdmax = conn->fd;
                }
            }
        }
        /* Add clients sockets to write array */
        for (lisr = conf->listeners; lisr; lisr = lisr->next) {
            for (conn = lisr->connects; conn; conn = conn->next) {
                if (!conn->want_to_write) {
                    continue;
                }
                FD_SET(conn->fd, &wfds);
                if (fdmax < conn->fd) {
                    fdmax = conn->fd;
                }
            }
        }

        timeout.tv_sec = 10; // TODO: get value from config
        timeout.tv_usec = 0;

        flag = select(fdmax + 1, &rfds, &wfds, NULL, &timeout);
        if (flag == -1) {
            /* select() failed */
            if (errno != EINTR) {
                fprintf(stderr, "select() failed\n");
                abort();
            }
            /* Interrupted by signal */
            else {
                printf("new input signal\n");
            }
        }
        /* Timeout */
        if (!flag) {
            printf("timeout\n");
        }

        for (lisr = conf->listeners; lisr; lisr = lisr->next) {
            /* Search listeners */
            if (FD_ISSET(lisr->fd, &rfds)) {
                printf("New connection %s:%d\n", lisr->ip, lisr->port);
                err = create_and_accept_connection(lisr);
                if (err != OK) {
                    fprintf(stderr, "create_and_accept_connection() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
            /* Search current listener connections */
            for (conn = lisr->connects; conn; conn = conn->next) {
                /* New data available to read from client */
                if (FD_ISSET(conn->fd, &rfds)) {
                    printf("new data from %d\n", get_connect_port(conn));
                }
                /* Client buffer available to write data */
                if (FD_ISSET(conn->fd, &wfds)) {
                    printf("wait data to %d\n", get_connect_port(conn));
                }
            }
        }
    }
}
