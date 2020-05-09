#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_event_actions_t hcnse_event_actions;


void
hcnse_process_events_and_timers(hcnse_conf_t *conf)
{
#if (HCNSE_HAVE_SELECT)
    hcnse_event_actions = hcnse_event_actions_select;
#elif (HCNSE_HAVE_KQUEUE && HCNSE_FREEBSD)
    hcnse_event_actions = hcnse_event_actions_kqueue;
#elif (HCNSE_HAVE_EPOLL && HCNSE_LINUX)
    hcnse_event_actions = hcnse_event_actions_epoll;
#elif (HCNSE_HAVE_IOCP && HCNSE_WIN32)
    hcnse_event_actions = hcnse_event_actions_iocp;
#endif


    hcnse_init_event_actions(conf);


    while (1) {

        hcnse_process_events(conf);



    } /* while (1) */







}



// hcnse_err_t
// hcnse_stop_process_events_and_timers(void)
// {
//     /* Raise the signal which interrupt cycle */
//     return HCNSE_OK;
// }
