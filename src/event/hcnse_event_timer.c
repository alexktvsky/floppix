#include "hcnse_portable.h"
#include "hcnse_common.h"


hcnse_err_t
hcnse_event_timer(hcnse_conf_t *conf)
{
    (void) conf;

    hcnse_log_error(HCNSE_LOG_INFO, conf->log, HCNSE_OK, "Timeout");
    return HCNSE_OK;
}
