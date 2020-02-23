#include <stddef.h>
#include <string.h>

#include "errors.h"

#define ERR_UNDEF_CODE 0


static const char *message_ok = "OK";

static const struct {
    err_t code;
    const char *message;
} error_list[] = {
    /* ERR_CONF domain */
    {ERR_CONF_OPEN,              "Failed to open config file"},
    {ERR_CONF_SIZE,              "Failed to determine config file size"},
    {ERR_CONF_READ,              "Failed to read from config file"},
    {ERR_CONF_REGEX,             "Failed to compile regular expressions"},

    /* ERR_NET domain */
    {ERR_NET_SOCKET,             "Failed to initialize socket"},
    {ERR_NET_BIND,               "Failed to bind socket"},
    {ERR_NET_TCP_NOPUSH,         "Failed to set socket in no push mode"},
    {ERR_NET_TCP_PUSH,           "Failed to set socket in push mode"},
    {ERR_NET_TCP_NONBLOCK,       "Failed to set socket in nonblocking mode"},
    {ERR_NET_TCP_BLOCK,          "Failed to set socket in blocking mode"},
    {ERR_NET_LISTEN,             "Failed to mark socket as listening"},
    {ERR_NET_ACCEPT,             "Failed to accept a connection on a socket"},
    {ERR_NET_GAI,                "Failed to translate network address"},
    {ERR_NET_GSN,                "Failed to get address to which the socket is bound"},
    {ERR_NET_GPN,                "Failed to get address of the peer connected to the socket"},

    /* ERR_MEM domain */
    {ERR_MEM_NULL,               "A null pointer dereference"},
    {ERR_MEM_ALLOC,              "Failed to allocate memory"},
    {ERR_MEM_INIT_POOL,          "Failed to initialize memory pool"},

    /* ERR_LOG domain */
    {ERR_LOG_OPEN,               "Failed to open log file"},
    {ERR_LOG_WRITE,              "Failed to write to log file"},
    {ERR_LOG_MAXSIZE,            "Size of log file is too small"},

    /* ERR_SSL domain */
    {ERR_SSL_INIT,               "Failed to initialize SSL library"},
    {ERR_SSL_OPEN_CERT,          "Failed to open SSL certificate file"},
    {ERR_SSL_READ_CERT,          "Failed to read from SSL certificate file"},
    {ERR_SSL_OPEN_CERTKEY,       "Failed to open SSL certificate key file"},
    {ERR_SSL_READ_CERTKEY,       "Failed to read from SSL certificate key file"},

    /* ERR_PROC domain */
    {ERR_PROC_DAEMON,            "Failed to create server process"},
    {ERR_PROC_WORKDIR,           "Failed to set process workdir"},

    {ERR_FAILED,                 "Default system error"},
    /* End of error list */
    {ERR_UNDEF_CODE,             "Undefined error code"}
};


size_t err_strerror_r(err_t errcode, char *buf, size_t bufsize)
{
    size_t len;
    size_t n;
    if (errcode == ERR_OK) {
        len = strlen(message_ok);
        if (len > bufsize) {
            memmove(buf, message_ok, bufsize);
            n = bufsize;
        }
        else {
            memmove(buf, message_ok, len);
            n = len;
        }
        return n;
    }
    
    for (size_t i = 0; ; i++) {
        if (error_list[i].code == errcode ||
                    error_list[i].code == ERR_UNDEF_CODE) {
            len = strlen(error_list[i].message);
            if (len > bufsize) {
                memmove(buf, error_list[i].message, bufsize);
                n = bufsize;
            }
            else {
                memmove(buf, error_list[i].message, len);
                n = len;
            }
            break;
        }
    }
    return n;
}

const char *err_strerror(err_t errcode)
{
    if (errcode == ERR_OK) {
        return message_ok;
    }
    for (size_t i = 0; ; i++) {      
        if (error_list[i].code == errcode ||
                    error_list[i].code == ERR_UNDEF_CODE) {
            return error_list[i].message;
        }
    }
    return NULL;
}
