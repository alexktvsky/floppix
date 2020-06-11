#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

#define DEFAULT_CONFIG_FILE "server.conf"


hcnse_err_t init_conf(void);
hcnse_err_t set_conf_filename(const char *in_fname);
hcnse_err_t parse_conf(void);
void fini_conf(void);

listen_unit_t *conf_get_listeners(void);
size_t conf_get_nprocs(void);
char *conf_get_logfile(void);
size_t conf_get_maxlog(void);

#endif /* INCLUDED_CONFIG_H */
