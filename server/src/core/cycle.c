#include "cycle.h"
#include "events.h"


void process_events(config_t *conf)
{
#ifdef USE_SELECT_MODULE
    select_process_events(conf);
#else
#if (SYSTEM_LINUX)
    epoll_process_events(conf);
#elif (SYSTEM_FREEBSD)
    kqueue_process_events(conf);
#elif (SYSTEM_WINDOWS)
    iocp_process_events(conf);
#endif
#endif
}


void single_process_cycle(config_t *conf)
{

    process_events(conf);

}
