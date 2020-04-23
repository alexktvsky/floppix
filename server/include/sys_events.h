#ifndef INCLUDED_SYS_EVENTS_H
#define INCLUDED_SYS_EVENTS_H

#include "syshead.h"
#include "errors.h"
#include "config.h"

// void set_read_handler(void *);
// void set_write_handler(void *);
// void set_accept_handler(void *);
// void set_timer_handler(void *);


err_t start_process_events(config_t *conf);
err_t stop_process_events(void);


/* Don't use this functions directly */
void select_process_events(config_t *conf);
void epoll_process_events(config_t *conf);
void kqueue_process_events(config_t *conf);
void iocp_process_events(config_t *conf);

#endif /* INCLUDED_SYS_EVENTS_H */
