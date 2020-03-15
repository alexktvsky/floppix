#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H


typedef struct config_s config_t;


struct config_s {
    file_t *file;
    char *data;
    list_t *listeners;
    list_t *free_connects; // Linked list with free and available connections
    char *workdir;
    log_t *log;
    char *log_file;
    uint8_t log_level;
    size_t log_size;
    bool ssl_on;
    char *ssl_certfile;
    char *ssl_keyfile;
    int8_t priority; // XXX: Signed value
};


err_t config_init(config_t **conf, const char *fname);
void config_fini(config_t *conf);

#endif /* INCLUDED_CONFIG_H */
