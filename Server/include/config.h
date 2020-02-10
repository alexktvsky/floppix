#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H


typedef struct config_s config_t;


struct config_s {
    file_t *file;
    char *data;
    listening_t *listeners;
    char *log;
    char *workdir;
    uint8_t loglevel;
    size_t logsize;
    char *cert;
    char *cert_key;
};


err_t config_init(config_t **conf, const char *fname);
void config_fini(config_t *conf);

#endif /* INCLUDED_CONFIG_H */
