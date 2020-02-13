#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "syshead.h"
#include "errors.h"
#include "mempool.h"
#include "list.h"
#include "connection.h"
#include "files.h"
#include "syslog.h"
#include "config.h"
#include "cycle.h"
#include "events.h"


void event_timer(config_t *conf)
{
    printf("timeout\n");
    return;
}
