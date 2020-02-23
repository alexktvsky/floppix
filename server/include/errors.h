#ifndef INCLUDED_ERRORS_H
#define INCLUDED_ERRORS_H

#include <stddef.h>

#define ERR_OK                     0
#define ERR_FAILED                -1
#define ERR_BUSY                  -2
#define ERR_ABORT                 -3

#ifndef OK
#define OK ERR_OK
#endif

/* Error domain provides a unique value for all error codes */
#define ERROR_DOMAIN_BASE          100
#define ERROR_DOMAIN(domain) (domain * ERROR_DOMAIN_BASE)
#define SET_ERROR_DOMAIN(errcode) (errcode / ERROR_DOMAIN_BASE)

#define ERR_CONF                   ERROR_DOMAIN(1)
#define ERR_CONF_OPEN              (ERR_CONF+1)
#define ERR_CONF_SIZE              (ERR_CONF+2)
#define ERR_CONF_READ              (ERR_CONF+3)
#define ERR_CONF_REGEX             (ERR_CONF+4)

#define ERR_NET                    ERROR_DOMAIN(2)
#define ERR_NET_SOCKET             (ERR_NET+1)
#define ERR_NET_BIND               (ERR_NET+2)
#define ERR_NET_TCP_NOPUSH         (ERR_NET+3)
#define ERR_NET_TCP_PUSH           (ERR_NET+4)
#define ERR_NET_TCP_NONBLOCK       (ERR_NET+5)
#define ERR_NET_TCP_BLOCK          (ERR_NET+6)
#define ERR_NET_LISTEN             (ERR_NET+7)
#define ERR_NET_ACCEPT             (ERR_NET+8)
#define ERR_NET_GAI                (ERR_NET+9)
#define ERR_NET_GSN                (ERR_NET+10)
#define ERR_NET_GPN                (ERR_NET+11)

#define ERR_MEM                    ERROR_DOMAIN(3)
#define ERR_MEM_NULL               (ERR_MEM+1)
#define ERR_MEM_ALLOC              (ERR_MEM+2)
#define ERR_MEM_INIT_POOL          (ERR_MEM+3)

#define ERR_LOG                    ERROR_DOMAIN(4)
#define ERR_LOG_OPEN               (ERR_LOG+1)
#define ERR_LOG_WRITE              (ERR_LOG+2)
#define ERR_LOG_MAXSIZE            (ERR_LOG+3)

#define ERR_SSL                    ERROR_DOMAIN(5)
#define ERR_SSL_INIT               (ERR_SSL+1)
#define ERR_SSL_OPEN_CERT          (ERR_SSL+2)
#define ERR_SSL_READ_CERT          (ERR_SSL+3)
#define ERR_SSL_OPEN_CERTKEY       (ERR_SSL+4)
#define ERR_SSL_READ_CERTKEY       (ERR_SSL+5)

#define ERR_PROC                   ERROR_DOMAIN(7)
#define ERR_PROC_DAEMON            (ERR_PROC+1)
#define ERR_PROC_WORKDIR           (ERR_PROC+2)


/* Type for specifying an error or status code */
typedef int err_t;

const char *err_strerror(err_t errcode);
size_t err_strerror_r(err_t errcode, char *buf, size_t bufsize);

#endif /* INCLUDED_ERRORS_H */
