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
#include "list.h"
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
    int flag;
    socket_t fdmax;
    socket_t fd;
    err_t err;

    listener_t *ls;
    connect_t *cn;
    listnode_t *i;
    listnode_t *j;
    listnode_t *temp;

    list_t *free_connects; // linked list with free and available connections



    char str_ip[NI_MAXHOST]; // only for debug
    char str_port[NI_MAXSERV]; // only for debug

    err = list_create(&free_connects);
    if (err != OK) {
        fprintf(stderr, "list_create() failed\n");
        fprintf(stderr, "%s\n", get_strerror(err));
        abort();
    }

    while (1) {

        fdmax = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        for (i = list_first(conf->listeners); i; i = list_next(i)) {
            /* Add listening sockets to read array */
            fd = ((listener_t *)list_data(i))->fd;
            FD_SET(fd, &rfds);
            if (fdmax < fd) {
                fdmax = fd;
            }

            j = list_first(((listener_t *)list_data(i))->connects);
            for ( ; j; j = list_next(j)) {
                /* Add clients sockets to read array */
                cn = (connect_t *)list_data(j);
                fd = cn->fd;
                FD_SET(fd, &rfds);
                if (fdmax < fd) {
                    fdmax = fd;
                }

                /* Add clients sockets to write array */
                if (!cn->want_to_write) {
                    continue;
                }
                fd = cn->fd;
                FD_SET(fd, &wfds);
                if (fdmax < fd) {
                    fdmax = fd;
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

        for (i = list_first(conf->listeners); i; i = list_next(i)) {
            /* Search listeners */
            ls = (listener_t *)list_data(i);
            if (FD_ISSET(ls->fd, &rfds)) {
                printf("New connection %s:%s\n",
                    get_addr(str_ip, &ls->sockaddr),
                    get_port(str_port, &ls->sockaddr));


                temp = list_first(free_connects);

                /* XXX: If free_connects list is empty */
                if (!temp) {
                    err = connection_create(&cn);
                    if (err != OK) {
                        fprintf(stderr, "connection_create() failed\n");
                        fprintf(stderr, "%s\n", get_strerror(err));
                        abort();
                    }
                }
                else {
                    cn = list_data(temp);
                }
                err = connection_accept(cn, ls);
                if (err != OK) {
                    fprintf(stderr, "connection_accept() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }

                err = list_append(ls->connects, cn);
                if (err != OK) {
                    fprintf(stderr, "list_append() failed\n");
                    fprintf(stderr, "%s\n", get_strerror(err));
                    abort();
                }
            }
            /* Search from current listener connections */
            j = list_first(((listener_t *)list_data(i))->connects);
            for ( ; j; j = list_next(j)) {
                /* New data available to read from client */
                cn = (connect_t *)list_data(j);
                if (FD_ISSET(cn->fd, &rfds)) {
                    printf("New data from %s:%s\n",
                        get_addr(str_ip, &cn->sockaddr),
                        get_port(str_port, &cn->sockaddr));
                }
                /* Client buffer available to write data */
                if (FD_ISSET(cn->fd, &wfds)) {
                    printf("Wait data to %s:%s\n",
                        get_addr(str_ip, &cn->sockaddr),
                        get_port(str_port, &cn->sockaddr));
                }
            }
        }
    }
}
