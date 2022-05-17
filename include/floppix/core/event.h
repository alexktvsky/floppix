#ifndef FPX_CORE_EVENT_H
#define FPX_CORE_EVENT_H

#define fpx_add_listener       fpx_event_actions.add_listener
#define fpx_del_listener       fpx_event_actions.del_listener
#define fpx_add_connect        fpx_event_actions.add_connect
#define fpx_del_connect        fpx_event_actions.del_connect
#define fpx_process_events     fpx_event_actions.process_events
#define fpx_init_event_actions fpx_event_actions.init

struct fpx_event_s {
    void *data;
    unsigned int read : 1;
    unsigned int write : 1;
    unsigned int ready : 1;

    /* ... */

    fpx_event_handler_t handler;
};

struct fpx_event_actions_s {
    fpx_err_t (*add_listener)(fpx_listener_t *listener, int flags);
    fpx_err_t (*del_listener)(fpx_listener_t *listener, int flags);
    fpx_err_t (*add_connect)(fpx_conf_t *conf, fpx_listener_t *listener);
    fpx_err_t (*del_connect)(fpx_conf_t *conf, fpx_connect_t *connect);
    fpx_err_t (*process_events)(fpx_conf_t *conf);
    fpx_err_t (*init)(fpx_conf_t *conf);
};

extern fpx_event_actions_t fpx_event_actions;

void fpx_process_events_and_timers(fpx_cycle_t *cycle);

fpx_err_t fpx_event_connect(fpx_conf_t *conf, fpx_listener_t *listener);
fpx_err_t fpx_event_read(fpx_conf_t *conf, fpx_connect_t *connect);
fpx_err_t fpx_event_write(fpx_conf_t *conf, fpx_connect_t *connect);
fpx_err_t fpx_event_timer(fpx_conf_t *conf);

#endif /* FPX_CORE_EVENT_H */
