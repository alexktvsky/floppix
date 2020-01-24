#ifndef XXX_CONFIG_H
#define XXX_CONFIG_H

#define DEFAULT_CONFIG_FILE "server.conf"


xxx_err_t init_config(void);
xxx_err_t set_config_filename(const char *in_fname);
xxx_err_t parse_config(void);
void fini_config(void);

listen_unit_t *config_get_listeners(void);
size_t config_get_nprocs(void);
char *config_get_logfile(void);
size_t config_get_maxlog(void);

#endif /* XXX_CONFIG_H */
