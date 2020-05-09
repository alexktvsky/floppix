#include "hcnse_portable.h"
#include "hcnse_core.h"


void
hcnse_single_process_cycle(hcnse_conf_t *conf)
{

    hcnse_process_events_and_timers(conf);
}
