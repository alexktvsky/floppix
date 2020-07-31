#include "hcnse_portable.h"
#include "hcnse_common.h"


hcnse_err_t
hcnse_event_write(hcnse_conf_t *conf, hcnse_connect_t *connect)
{
    (void) conf;
    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];
    hcnse_connection_get_addr(str_ip, &connect->sockaddr);
    hcnse_connection_get_port(str_port, &connect->sockaddr);

    hcnse_log_error(HCNSE_LOG_INFO, conf->log, HCNSE_OK,
                                    "Wait data to %s:%s", str_ip, str_port);

    return HCNSE_OK;
}
