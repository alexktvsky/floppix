#ifndef LOG_H
#define LOG_H

#define LOG_EMERG  0
#define LOG_ERROR  1
#define LOG_WARN   2
#define LOG_INFO   3
#define LOG_DEBUG  4

status_t init_log(char *in_filename, ssize_t in_maxsize, int in_level_boundary);
status_t log_msg(int level, char *message);
status_t log_status(int level, status_t statcode);
void fini_log(void);

#endif /* LOG_H */
