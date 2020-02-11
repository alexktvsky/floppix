#ifndef INCLUDED_ERRORS_H
#define INCLUDED_ERRORS_H

#include <stddef.h>


/* Type for specifying an error or status code */
typedef int err_t;

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


const char *get_strerror(err_t errcode);
void cpystrerror(err_t errcode, char *buf, size_t bufsize);


#define ERR_CONF                   ERROR_DOMAIN(1)
#define ERR_CONF_REGEX             (ERR_CONF+1)

#define ERR_NET                    ERROR_DOMAIN(2)
#define ERR_NET_SOCKET             (ERR_NET+1)
#define ERR_NET_ADDR               (ERR_NET+2)
#define ERR_NET_BIND               (ERR_NET+3)
#define ERR_NET_TCP_NOPUSH         (ERR_NET+4)
#define ERR_NET_TCP_PUSH           (ERR_NET+5)
#define ERR_NET_TCP_NONBLOCK       (ERR_NET+6)
#define ERR_NET_TCP_BLOCK          (ERR_NET+7)
#define ERR_NET_LISTEN             (ERR_NET+8)
#define ERR_NET_ACCEPT             (ERR_NET+9)
#define ERR_NET_IPV6               (ERR_NET+10)

#define ERR_MEM                    ERROR_DOMAIN(3)
#define ERR_MEM_NULL_ADDR          (ERR_MEM+1)
#define ERR_MEM_ALLOC              (ERR_MEM+2)
#define ERR_MEM_INIT_POOL          (ERR_MEM+3)

#define ERR_LOG                    ERROR_DOMAIN(4)
#define ERR_LOGSIZE                (ERR_LOG+1)

#define ERR_FILE                   ERROR_DOMAIN(5)
#define ERR_FILE_OPEN              (ERR_FILE+1)
#define ERR_FILE_SIZE              (ERR_FILE+2)
#define ERR_FILE_READ              (ERR_FILE+3)
#define ERR_FILE_WRITE             (ERR_FILE+4)

#endif /* INCLUDED_ERRORS_H */
