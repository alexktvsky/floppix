#include "server/cycle.h"
#include "server/events.h"
#include "os/events.h"


void hcnse_single_process_cycle(hcnse_conf_t *conf)
{

    hcnse_start_process_events(conf);

}
