#ifndef LOG_H
#define LOG_H

#define LOG_ERROR 0
#define LOG_WARN  1
#define LOG_INFO  2
#define LOG_DEBUG 3

status_t init_log(char *in_filename, ssize_t in_maxsize, int in_level_boundary);
status_t log_msg(int level, char *message);
status_t log_error(status_t statcode);
void fini_log(void);

#endif /* LOG_H */
