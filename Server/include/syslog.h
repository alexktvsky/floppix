#ifndef SYSLOG_H
#define SYSLOG_H

#define LOG_EMERG  0
#define LOG_ERROR  1
#define LOG_WARN   2
#define LOG_INFO   3
#define LOG_DEBUG  4

#define DEFAULT_LOG_FILE "server.log"


status_t init_log(char *in_filename, ssize_t in_maxsize, int in_level_boundary);
status_t init_log_if_not(char *in_filename, ssize_t in_maxsize, int in_level_boundary);
status_t log_msg(int level, char *message);
status_t log_status(int level, status_t statcode);
void log_and_abort(int level, char *stage_description, status_t statcode);
void fini_log(void);

#endif /* SYSLOG_H */
