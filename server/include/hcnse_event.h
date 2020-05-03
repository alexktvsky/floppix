#ifndef INCLUDED_HCNSE_EVENT_H
#define INCLUDED_HCNSE_EVENT_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

hcnse_err_t hcnse_start_process_events(hcnse_conf_t *conf);
hcnse_err_t hcnse_stop_process_events(void);

/* Don't use this functions directly */
#if (HCNSE_HAVE_SELECT)
void hcnse_select_process_events(hcnse_conf_t *conf);
#endif

#if (HCNSE_HAVE_EPOLL && HCNSE_LINUX)
void hcnse_epoll_process_events(hcnse_conf_t *conf);
#endif

#if (HCNSE_HAVE_KQUEUE && HCNSE_FREEBSD)
void hcnse_kqueue_process_events(hcnse_conf_t *conf);
#endif

#if (HCNSE_HAVE_IOCP && HCNSE_WINDOWS)
void hcnse_iocp_process_events(hcnse_conf_t *conf);
#endif


hcnse_err_t hcnse_event_connect(hcnse_conf_t *conf, hcnse_listener_t *listener);
hcnse_err_t hcnse_event_read(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t hcnse_event_write(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t hcnse_event_timer(hcnse_conf_t *conf);

#endif /* INCLUDED_HCNSE_EVENT_H */
