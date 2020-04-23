#ifndef INCLUDED_EVENTS_H
#define INCLUDED_EVENTS_H

#include "error.h"
#include "list.h"
#include "connection.h"
#include "config.h"

err_t event_connect(config_t *conf, listener_t *listener);
err_t event_read(config_t *conf, connect_t *connect);
err_t event_write(config_t *conf, connect_t *connect);
err_t event_timer(config_t *conf);

#endif /* INCLUDED_EVENTS_H */
