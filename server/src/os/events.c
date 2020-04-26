#include "os/events.h"


hcnse_err_t hcnse_start_process_events(hcnse_conf_t *conf)
{
#if (EVENTS_USE_SELECT)
    select_process_events(conf);
#else
#if (HCNSE_LINUX)
    epoll_process_events(conf);
#elif (HCNSE_FREEBSD)
    kqueue_process_events(conf);
#elif (HCNSE_WINDOWS)
    iocp_process_events(conf);
#endif
#endif
    return HCNSE_OK;
}


hcnse_err_t hcnse_stop_process_events(void)
{
    /* Raise the signal which interrupt cycle */
    return HCNSE_OK;
}
