#ifndef INCLUDED_EVENTS_H
#define INCLUDED_EVENTS_H

err_t event_connect(config_t *conf, listener_t *ls);
err_t event_read(config_t *conf, connect_t *cn, listener_t *ls);
err_t event_write(config_t *conf, connect_t *cn, listener_t *ls);
void event_timer(config_t *conf);

#endif /* INCLUDED_EVENTS_H */
