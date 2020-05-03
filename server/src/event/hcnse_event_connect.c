#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t hcnse_event_connect(hcnse_conf_t *conf, hcnse_listener_t *listener)
{
    (void) conf;

    char str_ip[NI_MAXHOST];
    char str_port[NI_MAXSERV];
    hcnse_connection_get_addr(str_ip, &listener->sockaddr);
    hcnse_connection_get_port(str_port, &listener->sockaddr);

    hcnse_log_error(HCNSE_LOG_INFO, conf->log, HCNSE_OK,
                                    "New connection %s:%s", str_ip, str_port);

    return HCNSE_OK;
}
