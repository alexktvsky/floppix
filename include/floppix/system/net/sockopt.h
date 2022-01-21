#ifndef FLOPPIX_SYSTEM_NET_SOCKOPT_H
#define FLOPPIX_SYSTEM_NET_SOCKOPT_H

#include "floppix/system/os/portable.h"
#include "floppix/system/errno.h"

fpx_err_t fpx_socket_nopush(fpx_socket_t s);
fpx_err_t fpx_socket_push(fpx_socket_t s);
fpx_err_t fpx_socket_nonblocking(fpx_socket_t s);
fpx_err_t fpx_socket_blocking(fpx_socket_t s);

#endif /* FLOPPIX_SYSTEM_NET_SOCKOPT_H */
