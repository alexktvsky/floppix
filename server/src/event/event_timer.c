#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "events.h"


void event_timer(config_t *conf)
{
    printf("timeout\n");
    return;
}
