#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t hcnse_start_process_events(hcnse_conf_t *conf)
{
#if (HCNSE_HAVE_SELECT)
    hcnse_select_process_events(conf);

#elif (HCNSE_HAVE_EPOLL)
    hcnse_epoll_process_events(conf);

#elif (HCNSE_HAVE_KQUEUE)
    hcnse_kqueue_process_events(conf);

#elif (HCNSE_HAVE_IOCP)
    hcnse_iocp_process_events(conf);

#endif
    return HCNSE_OK;
}


hcnse_err_t hcnse_stop_process_events(void)
{
    /* Raise the signal which interrupt cycle */
    return HCNSE_OK;
}
