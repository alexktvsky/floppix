#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_CONFIG_FILE "config.conf"


status_t init_config(void);
status_t set_config_filename(char *in_filename);
status_t parse_config(void);
void fini_config(void);

listen_unit_t *config_get_listeners(void);
size_t config_get_nprocs(void);
char *config_get_logfile(void);
size_t config_get_maxlog(void);

#endif /* CONFIG_H */
