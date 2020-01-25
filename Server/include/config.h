#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

#define DEFAULT_CONFIG_FILE "server.conf"


err_t init_config(void);
err_t set_config_filename(const char *in_fname);
err_t parse_config(void);
void fini_config(void);

listen_unit_t *config_get_listeners(void);
size_t config_get_nprocs(void);
char *config_get_logfile(void);
size_t config_get_maxlog(void);

#endif /* INCLUDED_CONFIG_H */
