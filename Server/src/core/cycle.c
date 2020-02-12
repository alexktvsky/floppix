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
    listener_t *ls;
    connect_t *conn;
    int flag;
    socket_t fdmax;
    err_t err;

    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug


    /* Init connection lists for all listeners */
    for (ls = conf->listeners->head; ls; ls = ls->next) {
        err = conn_list_create(&ls->connects);
        if (err != OK) {
            fprintf(stderr, "conn_list_create() failed\n");
            fprintf(stderr, "%s\n", get_strerror(err));
            abort();
        }
    }

    while (1) {

        fdmax = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        /* Add listening sockets to read array */
        for (ls = conf->listeners->head; ls; ls = ls->next) {
            FD_SET(ls->fd, &rfds);
            if (fdmax < ls->fd) {
                fdmax = ls->fd;
            }
        }
        /* Add clients sockets to read array */
        for (ls = conf->listeners->head; ls; ls = ls->next) {
            for (conn = ls->connects->head; conn; conn = conn->next) {
                FD_SET(conn->fd, &rfds);
                if (fdmax < conn->fd) {
                    fdmax = conn->fd;
                }
            }
        }
        /* Add clients sockets to write array */
        for (ls = conf->listeners->head; ls; ls = ls->next) {
            for (conn = ls->connects->head; conn; conn = conn->next) {
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

        for (ls = conf->listeners->head; ls; ls = ls->next) {
            /* Search listeners */
            if (FD_ISSET(ls->fd, &rfds)) {

                printf("New connection %s:%s\n", get_addr(str_ip, &ls->sockaddr),
                                get_port(str_port, &ls->sockaddr));
                err = conn_list_append(ls->connects, ls);
                if (err != OK) {
                    fprintf(stderr, "conn_list_append() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
            /* Search current listener connections */
            for (conn = ls->connects->head; conn; conn = conn->next) {
                /* New data available to read from client */
                if (FD_ISSET(conn->fd, &rfds)) {
                    printf("New data from %s:%s\n", get_addr(str_ip, &conn->sockaddr),
                                get_port(str_port, &conn->sockaddr));
                }
                /* Client buffer available to write data */
                if (FD_ISSET(conn->fd, &wfds)) {
                    printf("Wait data to %s:%s\n", get_addr(str_ip, &conn->sockaddr),
                                get_port(str_port, &conn->sockaddr));
                }
            }
        }
    }
}
