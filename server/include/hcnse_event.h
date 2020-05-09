#ifndef INCLUDED_HCNSE_EVENT_H
#define INCLUDED_HCNSE_EVENT_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define hcnse_add_listener        hcnse_event_actions.add_listener
#define hcnse_del_listener        hcnse_event_actions.del_listener
#define hcnse_add_connect         hcnse_event_actions.add_connect
#define hcnse_del_connect         hcnse_event_actions.del_connect
#define hcnse_process_events      hcnse_event_actions.process_events
#define hcnse_init_event_actions  hcnse_event_actions.init


struct hcnse_event_s {
    void *data;
    bool read;
    bool write;
    bool ready;

    /* ... */

    hcnse_event_handler_t handler;
};

struct hcnse_event_actions_s {
    hcnse_err_t (*add_listener)(hcnse_listener_t *listener, int flags);
    hcnse_err_t (*del_listener)(hcnse_listener_t *listener, int flags);
    hcnse_err_t (*add_connect)(hcnse_conf_t *conf, hcnse_listener_t *listener);
    hcnse_err_t (*del_connect)(hcnse_conf_t *conf, hcnse_connect_t *connect);
    hcnse_err_t (*process_events)(hcnse_conf_t *conf);
    hcnse_err_t (*init)(hcnse_conf_t *conf);
};

extern hcnse_event_actions_t hcnse_event_actions;

#if (HCNSE_HAVE_SELECT)
extern hcnse_event_actions_t hcnse_event_actions_select;
#endif

#if (HCNSE_HAVE_EPOLL && HCNSE_LINUX)
extern hcnse_event_actions_t hcnse_event_actions_epoll;
#endif

#if (HCNSE_HAVE_KQUEUE && HCNSE_FREEBSD)
extern hcnse_event_actions_t hcnse_event_actions_kqueue;
#endif

#if (HCNSE_HAVE_IOCP && HCNSE_WIN32)
extern hcnse_event_actions_t hcnse_event_actions_iocp;
#endif


void hcnse_process_events_and_timers(hcnse_conf_t *conf);



hcnse_err_t hcnse_event_connect(hcnse_conf_t *conf, hcnse_listener_t *listener);
hcnse_err_t hcnse_event_read(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t hcnse_event_write(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t hcnse_event_timer(hcnse_conf_t *conf);

#endif /* INCLUDED_HCNSE_EVENT_H */
