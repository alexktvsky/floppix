#ifndef XXX_SYSLOG_H
#define XXX_SYSLOG_H

#define LOG_EMERG  0
#define LOG_ERROR  1
#define LOG_WARN   2
#define LOG_INFO   3
#define LOG_DEBUG  4

#define DEFAULT_LOG_FILE "server.log"


status_t init_log(const char *in_fname, ssize_t in_maxsize, int in_level_boundary);
status_t log_msg_ex(FILE *in_openfile, int in_level, const char *message);
status_t log_msg(int level, const char *message);
status_t log_status(int level, status_t statcode);
void log_error(int level, const char *description, status_t statcode);
void fini_log(void);

#endif /* XXX_SYSLOG_H */
