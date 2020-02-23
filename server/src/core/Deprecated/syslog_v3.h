#ifndef INCLUDED_SYSLOG_H
#define INCLUDED_SYSLOG_H

#define LOG_EMERG   0
#define LOG_ERROR   1
#define LOG_WARN    2
#define LOG_INFO    3
#define LOG_DEBUG   4

#define DEFAULT_LOGFILE "server.log"


err_t init_log(const char *in_fname, ssize_t in_maxsize, int in_level_boundary);
err_t log_msg_ex(FILE *in_openfile, int in_level, const char *message);
err_t log_msg(int level, const char *message);
err_t log_status(int level, err_t errcode);
void log_error(int level, const char *description, err_t errcode);
void fini_log(void);

#endif /* INCLUDED_SYSLOG_H */
