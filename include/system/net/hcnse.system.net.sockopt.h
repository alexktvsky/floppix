#ifndef HCNSE_SYSTEM_NET_SOCKOPT_H
#define HCNSE_SYSTEM_NET_SOCKOPT_H

#include "hcnse.system.os.portable.h"
#include "hcnse.system.errno.h"

hcnse_err_t hcnse_socket_nopush(hcnse_socket_t s);
hcnse_err_t hcnse_socket_push(hcnse_socket_t s);
hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s);
hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s);

#endif /* HCNSE_SYSTEM_NET_SOCKOPT_H */
