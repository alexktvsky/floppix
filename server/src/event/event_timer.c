#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "events.h"


err_t event_timer(config_t *conf)
{
    (void) conf;
    printf("timeout\n");
    return OK;
}
