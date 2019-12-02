#ifndef ERROR_PROC_H
#define ERROR_PROC_H

#include <stddef.h>


/* Type for specifying an error or status code */
typedef int status_t;

#define XXX_OK                     0
#define XXX_FAILED                 1
#define XXX_BUSY                   2
#define XXX_ABORT                  3

/* Error domain provides a unique value for all error codes */
#define ERROR_DOMAIN_BASE          0x100
#define ERROR_DOMAIN(_domain) (_domain * ERROR_DOMAIN_BASE)

#define SET_ERROR_DOMAIN(_error) (_error / ERROR_DOMAIN_BASE)
#define IS_NOT_ERROR(_error) (_error != SUCCESS)


const char *set_strerror(status_t statcode);
void cpystrerror(status_t statcode, char *buf, size_t bufsize);


#define BOOT_ERROR                 ERROR_DOMAIN(1)

#define CONF_ERROR                 ERROR_DOMAIN(2)
#define CONF_INIT_ERROR            (CONF_ERROR+1)
#define CONF_OPEN_ERROR            (CONF_ERROR+2)
#define CONF_OVERFLOW_ERROR        (CONF_ERROR+3)
#define CONF_REGEX_ERROR           (CONF_ERROR+4)
#define CONF_SUBSTR_ERROR          (CONF_ERROR+5)
#define CONF_SYNTAX_ERROR          (CONF_ERROR+6)

#define NETWORK_ERROR              ERROR_DOMAIN(3)
#define INIT_SOCKET_ERROR          (NETWORK_ERROR+1)
#define ADDR_ERROR                 (NETWORK_ERROR+2)
#define SETSOCKOPT_ERROR           (NETWORK_ERROR+3)
#define BIND_ERROR                 (NETWORK_ERROR+4)
#define INIT_LISTEN_ERROR          (NETWORK_ERROR+5)
#define IPV6_NOT_SUPPORTED         (NETWORK_ERROR+6)

#define MEMORY_ERROR               ERROR_DOMAIN(4)
#define NULL_ADDRESS_ERROR         (MEMORY_ERROR+1)
#define ALLOC_MEM_ERROR            (MEMORY_ERROR+2)
#define INIT_POOL_ERROR            (MEMORY_ERROR+3)


#define SYSLOG_ERROR               ERROR_DOMAIN(5)
#define LOG_MAXSIZE_ERROR          (SYSLOG_ERROR+1)
#define LOG_OPEN_ERROR             (SYSLOG_ERROR+2)
#define LOG_WRITE_ERROR            (SYSLOG_ERROR+3)

#endif /* ERROR_PROC_H */
