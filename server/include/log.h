#ifndef INCLUDED_LOG_H
#define INCLUDED_LOG_H

#define LOG_EMERG   0
#define LOG_ERROR   1
#define LOG_WARN    2
#define LOG_INFO    3
#define LOG_DEBUG   4


typedef struct log_s log_t;

struct log_s {
    file_t *file;
    uint8_t level;
};


// void log_error(int level, const char *description, err_t errcode);


#endif /* INCLUDED_LOG_H */
