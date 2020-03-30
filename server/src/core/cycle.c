#include "cycle.h"
#include "events.h"


void process_events(config_t *conf)
{
    select_process_events(conf);
}


void single_process_cycle(config_t *conf)
{

    process_events(conf);

}
