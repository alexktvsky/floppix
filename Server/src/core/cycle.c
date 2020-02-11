#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "syshead.h"

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>


#elif (SYSTEM_WINDOWS)
#include <winsock2.h>
#endif

#include "errors.h"
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
    listener_t *lsock;
    int flag;
    int fdmax;

    while (1) {
        fdmax = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        for (lsock = conf->listeners; lsock; lsock = lsock->next) {
            FD_SET(lsock->fd, &rfds);
            if (fdmax < lsock->fd) {
                fdmax = lsock->fd;
            }
        }
        // for () {} add clients sockets to read
        // for () {} add clients sockets to write

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

        for (lsock = conf->listeners; lsock; lsock = lsock->next) {
            if (FD_ISSET(lsock->fd, &rfds)) {
                printf("new accept on %s\n", lsock->ip);
            }
        }
        // for () {} check clients sockets to read
        // for () {} check clients sockets to write

    }
}
