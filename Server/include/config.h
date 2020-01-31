#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

#define DEFAULT_CONF_FILE "server.conf"

typedef struct config_s {
    file_t *file;
    char *data;
    char *log;
    uint8_t loglevel;
    size_t logsize;
} config_t;




#endif /* INCLUDED_CONFIG_H */
