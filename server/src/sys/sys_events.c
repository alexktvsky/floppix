#include "sys_events.h"


err_t start_process_events(config_t *conf)
{
#if (EVENTS_USE_SELECT)
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
    return OK;
}


err_t stop_process_events(void)
{
    /* Raise the signal which interrupt cycle */
    return OK;
}
