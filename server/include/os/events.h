#ifndef INCLUDED_OS_EVENTS_H
#define INCLUDED_OS_EVENTS_H

#include "os/syshead.h"
#include "server/errors.h"
#include "server/config.h"

// void set_read_handler(void *);
// void set_write_handler(void *);
// void set_accept_handler(void *);
// void set_timer_handler(void *);


hcnse_err_t hcnse_start_process_events(hcnse_conf_t *conf);
hcnse_err_t hcnse_stop_process_events(void);


/* Don't use this functions directly */
void select_process_events(hcnse_conf_t *conf);
void epoll_process_events(hcnse_conf_t *conf);
void kqueue_process_events(hcnse_conf_t *conf);
void iocp_process_events(hcnse_conf_t *conf);

#endif /* INCLUDED_OS_EVENTS_H */
