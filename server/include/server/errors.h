#ifndef INCLUDED_ERRORS_H
#define INCLUDED_ERRORS_H

#include <stddef.h>

#define HCNSE_OK                       0
#define HCNSE_FAILED                  -1
#define HCNSE_BUSY                    -2
#define HCNSE_ABORT                   -3


/* Error domain provides a unique value for all error codes */
#define HCNSE_ERROR_DOMAIN_BASE        1000
#define HCNSE_ERROR_DOMAIN(domain) \
    (domain * HCNSE_ERROR_DOMAIN_BASE)
#define HCNSE_SET_ERROR_DOMAIN(errcode) \
    (errcode / HCNSE_ERROR_DOMAIN_BASE)

#define HCNSE_ERR_CONF                       HCNSE_ERROR_DOMAIN(1)
#define HCNSE_ERR_CONF_OPEN                  (HCNSE_ERR_CONF+1)
#define HCNSE_ERR_CONF_SIZE                  (HCNSE_ERR_CONF+2)
#define HCNSE_ERR_CONF_READ                  (HCNSE_ERR_CONF+3)
#define HCNSE_ERR_CONF_REGEX                 (HCNSE_ERR_CONF+4)

#define HCNSE_ERR_NET                        HCNSE_ERROR_DOMAIN(2)
#define HCNSE_ERR_NET_SOCKET                 (HCNSE_ERR_NET+1)
#define HCNSE_ERR_NET_BIND                   (HCNSE_ERR_NET+2)
#define HCNSE_ERR_NET_TCP_NOPUSH             (HCNSE_ERR_NET+3)
#define HCNSE_ERR_NET_TCP_PUSH               (HCNSE_ERR_NET+4)
#define HCNSE_ERR_NET_TCP_NONBLOCK           (HCNSE_ERR_NET+5)
#define HCNSE_ERR_NET_TCP_BLOCK              (HCNSE_ERR_NET+6)
#define HCNSE_ERR_NET_LISTEN                 (HCNSE_ERR_NET+7)
#define HCNSE_ERR_NET_ACCEPT                 (HCNSE_ERR_NET+8)
#define HCNSE_ERR_NET_GAI                    (HCNSE_ERR_NET+9)
#define HCNSE_ERR_NET_GSN                    (HCNSE_ERR_NET+10)
#define HCNSE_ERR_NET_GPN                    (HCNSE_ERR_NET+11)

#define HCNSE_ERR_MEM                        HCNSE_ERROR_DOMAIN(3)
#define HCNSE_ERR_MEM_NULL                   (HCNSE_ERR_MEM+1)
#define HCNSE_ERR_MEM_ALLOC                  (HCNSE_ERR_MEM+2)
#define HCNSE_ERR_MEM_INIT_POOL              (HCNSE_ERR_MEM+3)

#define HCNSE_ERR_LOG                        HCNSE_ERROR_DOMAIN(4)
#define HCNSE_ERR_LOG_OPEN                   (HCNSE_ERR_LOG+1)
#define HCNSE_ERR_LOG_WRITE                  (HCNSE_ERR_LOG+2)
#define HCNSE_ERR_LOG_MAXSIZE                (HCNSE_ERR_LOG+3)

#define HCNSE_ERR_SSL                        HCNSE_ERROR_DOMAIN(5)
#define HCNSE_ERR_SSL_INIT                   (HCNSE_ERR_SSL+1)
#define HCNSE_ERR_SSL_OPEN_CERT              (HCNSE_ERR_SSL+2)
#define HCNSE_ERR_SSL_READ_CERT              (HCNSE_ERR_SSL+3)
#define HCNSE_ERR_SSL_OPEN_CERTKEY           (HCNSE_ERR_SSL+4)
#define HCNSE_ERR_SSL_READ_CERTKEY           (HCNSE_ERR_SSL+5)

#define HCNSE_ERR_PROC                       HCNSE_ERROR_DOMAIN(7)
#define HCNSE_ERR_PROC_DAEMON                (HCNSE_ERR_PROC+1)
#define HCNSE_ERR_PROC_WORKDIR               (HCNSE_ERR_PROC+2)


/* Type for specifying an error or status code */
typedef int hcnse_err_t;

const char *hcnse_strerror(hcnse_err_t errcode);
size_t hcnse_strerror_r(hcnse_err_t errcode, char *buf, size_t bufsize);

#endif /* INCLUDED_ERRORS_H */
