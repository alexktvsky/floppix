#ifndef XXX_SYSLOG_H
#define XXX_SYSLOG_H

#define LOG_EMERG  0
#define LOG_ERROR  1
#define LOG_WARN   2
#define LOG_INFO   3
#define LOG_DEBUG  4

#define DEFAULT_LOG_FILE "server.log"


xxx_err_t init_log(const char *in_fname, ssize_t in_maxsize, int in_level_boundary);
xxx_err_t log_msg_ex(FILE *in_openfile, int in_level, const char *message);
xxx_err_t log_msg(int level, const char *message);
xxx_err_t log_status(int level, xxx_err_t errcode);
void log_error(int level, const char *description, xxx_err_t errcode);
void fini_log(void);

#endif /* XXX_SYSLOG_H */
