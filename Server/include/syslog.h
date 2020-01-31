#ifndef INCLUDED_SYSLOG_H
#define INCLUDED_SYSLOG_H

#define LOG_EMERG   0
#define LOG_ERROR   1
#define LOG_WARN    2
#define LOG_INFO    3
#define LOG_DEBUG   4

#define DEFAULT_LOGFILE "server.log"


// void log_error(int level, const char *description, err_t errcode);


#endif /* INCLUDED_SYSLOG_H */
