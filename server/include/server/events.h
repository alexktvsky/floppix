#ifndef INCLUDED_EVENTS_H
#define INCLUDED_EVENTS_H

#include "server/errors.h"
#include "server/list.h"
#include "server/connection.h"
#include "server/config.h"

hcnse_err_t event_connect(hcnse_conf_t *conf, hcnse_listener_t *listener);
hcnse_err_t event_read(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t event_write(hcnse_conf_t *conf, hcnse_connect_t *connect);
hcnse_err_t event_timer(hcnse_conf_t *conf);

#endif /* INCLUDED_EVENTS_H */
