#ifndef INCLUDED_EVENTS_H
#define INCLUDED_EVENTS_H

#include "error.h"
#include "list.h"
#include "connection.h"
#include "config.h"

err_t event_connect(config_t *conf, listener_t *listener);
err_t event_read(config_t *conf, connect_t *connect, listener_t *listener);
err_t event_write(config_t *conf, connect_t *connect, listener_t *listener);
void event_timer(config_t *conf);


void select_process_events(config_t *conf);
void epoll_process_events(config_t *conf);
void kqueue_process_events(config_t *conf);
void iocp_process_events(config_t *conf);

#endif /* INCLUDED_EVENTS_H */
