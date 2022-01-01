#ifndef FPX_SYSTEM_NET_SOCKOPT_H
#define FPX_SYSTEM_NET_SOCKOPT_H

#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"

fpx_err_t fpx_socket_nopush(fpx_socket_t s);
fpx_err_t fpx_socket_push(fpx_socket_t s);
fpx_err_t fpx_socket_nonblocking(fpx_socket_t s);
fpx_err_t fpx_socket_blocking(fpx_socket_t s);

#endif /* FPX_SYSTEM_NET_SOCKOPT_H */
