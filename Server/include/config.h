#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H


typedef struct config_s config_t;


struct config_s {
    file_t *file;
    char *data;
    list_t *listeners;
    list_t *free_connects; // linked list with free and available connections
    char *logfile;
    char *workdir;
    uint8_t loglevel;
    size_t logsize;
    log_t *log;
    bool use_ssl;
    char *cert;
    char *cert_key;
    int8_t priority; // signed value
};


err_t config_init(config_t **conf, const char *fname);
void config_fini(config_t *conf);

#endif /* INCLUDED_CONFIG_H */
