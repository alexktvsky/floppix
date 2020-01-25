#include <stddef.h>
#include <string.h>

#include "errors.h"

#define UNDEF_ERROR_CODE 0


static const struct {
    err_t code;
    const char *message;
} error_list[] = {
    /* CONF_ERROR domain */
    {CONF_INIT_ERROR,              "Error of initialization config"},
    {CONF_OPEN_ERROR,              "Config file isn't exist or available"},
    {CONF_OVERFLOW_ERROR,          "Parser section overflow"},
    {CONF_REGEX_ERROR,             "Failed to compiling regular expressions"},
    {CONF_SUBSTR_ERROR,            "Error of pcre substring list"},
    {CONF_SYNTAX_ERROR,            "Error of syntax configure file"},

    /* NETWORK_ERROR domain */
    {INIT_SOCKET_ERROR,            "Error of initialization new socket"},
    {ADDR_ERROR,                   "Host IP address is not correct"},
    {BIND_ERROR,                   "Error of binding."},
    {SETSOCKOPT_ERROR,             "Error of set options on sockets"},
    {INIT_LISTEN_ERROR,            "Error of initialization listening socket"},
    {IPV6_NOT_SUPPORTED,           "IPv6 is not supported on this system"},

    /* MEMORY_ERROR domain */
    {NULL_ADDRESS_ERROR,           "Error of attempted to access a NULL address"},
    {ALLOC_MEM_ERROR,              "Error of allocate memory"},
    {INIT_POOL_ERROR,              "Error of initialization new memory pool"},

    /* SYSLOG_ERROR domain */
    {LOG_MAXSIZE_ERROR,            "Size of log file is too small"},
    {LOG_OPEN_ERROR,               "Error of opening log file"},
    {LOG_WRITE_ERROR,              "Error of writing log file"},

    {ERR_FAILED,                   "Default system error"},
    /* End of error list */
    {UNDEF_ERROR_CODE,             "Undefined error code"}
};

static const char *message_ok = "OK";


void cpystrerror(err_t errcode, char *buf, size_t bufsize)
{
    size_t len;
    if (errcode == OK) {
        len = strlen(message_ok);
        if (len > bufsize) {
            memmove(buf, message_ok, bufsize);
        }
        else {
            memmove(buf, message_ok, len);
        }
        return;
    }
    
    for (size_t i = 0; ; i++) {
        if (error_list[i].code == errcode ||
                    error_list[i].code == UNDEF_ERROR_CODE) {
            len = strlen(error_list[i].message);
            if (len > bufsize) {
                memmove(buf, error_list[i].message, bufsize);
            }
            else {
                memmove(buf, error_list[i].message, len);
            }
            break;
        }
    }
    return;
}


const char *set_strerror(err_t errcode)
{
    if (errcode == OK) {
        return message_ok;
    }
    for (size_t i = 0; ; i++) {      
        if (error_list[i].code == errcode ||
                    error_list[i].code == UNDEF_ERROR_CODE) {
            return error_list[i].message;
        }
    }
    return NULL;
}
