#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "server/events.h"


hcnse_err_t event_timer(hcnse_conf_t *conf)
{
    (void) conf;
    printf("timeout\n");
    return HCNSE_OK;
}
