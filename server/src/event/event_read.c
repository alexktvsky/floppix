#include <stdint.h>
#include <stddef.h>

#include "server/events.h"
#include "server/log.h"
#include "server/errors.h" // HCNSE_EAGAIN


hcnse_err_t event_read(hcnse_conf_t *conf, hcnse_connect_t *connect)
{
    (void) conf;
    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];
    hcnse_connection_get_addr(str_ip, &connect->sockaddr);
    hcnse_connection_get_port(str_port, &connect->sockaddr);

    hcnse_log_error(HCNSE_LOG_INFO, conf->log, HCNSE_OK,
                                    "New data from %s:%s", str_ip, str_port);

    return HCNSE_OK;
}
