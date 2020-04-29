#include <stdint.h>
#include <stddef.h>

#include "server/log.h"
#include "server/events.h"


hcnse_err_t event_timer(hcnse_conf_t *conf)
{
    (void) conf;
    hcnse_log_debug(HCNSE_LOG_INFO, conf->log, "Timeout");

    return HCNSE_OK;
}
