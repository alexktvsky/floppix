#ifndef INCLUDED_HCNSE_SOCKOPT_H
#define INCLUDED_HCNSE_SOCKOPT_H

#include "hcnse_portable.h"
#include "hcnse_common.h"

hcnse_err_t hcnse_socket_nopush(hcnse_socket_t s);
hcnse_err_t hcnse_socket_push(hcnse_socket_t s);
hcnse_err_t hcnse_socket_nonblocking(hcnse_socket_t s);
hcnse_err_t hcnse_socket_blocking(hcnse_socket_t s);

#endif /* INCLUDED_HCNSE_SOCKOPT_H */
