#include <stddef.h>
#include <string.h>

#include "server/errors.h"

#define HCNSE_ERR_END_LIST 0


static const char *message_ok = "OK";

static const struct {
    hcnse_err_t code;
    const char *message;
} error_list[] = {
    /* HCNSE_ERR_CONF domain */
    {HCNSE_ERR_CONF_OPEN,            "Failed to open config file"},
    {HCNSE_ERR_CONF_SIZE,            "Failed to determine config file size"},
    {HCNSE_ERR_CONF_READ,            "Failed to read from config file"},
    {HCNSE_ERR_CONF_REGEX,           "Failed to compile regular expressions"},

    /* HCNSE_ERR_NET domain */
    {HCNSE_ERR_NET_SOCKET,           "Failed to initialize socket"},
    {HCNSE_ERR_NET_BIND,             "Failed to bind socket"},
    {HCNSE_ERR_NET_TCP_NOPUSH,       "Failed to set socket in no push mode"},
    {HCNSE_ERR_NET_TCP_PUSH,         "Failed to set socket in push mode"},
    {HCNSE_ERR_NET_TCP_NONBLOCK,     "Failed to set socket in nonblocking mode"},
    {HCNSE_ERR_NET_TCP_BLOCK,        "Failed to set socket in blocking mode"},
    {HCNSE_ERR_NET_LISTEN,           "Failed to mark socket as listening"},
    {HCNSE_ERR_NET_ACCEPT,           "Failed to accept a connection on a socket"},
    {HCNSE_ERR_NET_GAI,              "Failed to translate network address"},
    {HCNSE_ERR_NET_GSN,              "Failed to get address to which the socket is bound"},
    {HCNSE_ERR_NET_GPN,              "Failed to get address of the peer connected to the socket"},

    /* HCNSE_ERR_MEM domain */
    {HCNSE_ERR_MEM_NULL,             "A null pointer dereference"},
    {HCNSE_ERR_MEM_ALLOC,            "Failed to allocate memory"},
    {HCNSE_ERR_MEM_INIT_POOL,        "Failed to initialize memory pool"},

    /* HCNSE_ERR_LOG domain */
    {HCNSE_ERR_LOG_OPEN,             "Failed to open log file"},
    {HCNSE_ERR_LOG_WRITE,            "Failed to write to log file"},
    {HCNSE_ERR_LOG_MAXSIZE,          "Size of log file is too small"},

    /* HCNSE_ERR_SSL domain */
    {HCNSE_ERR_SSL_INIT,             "Failed to initialize SSL library"},
    {HCNSE_ERR_SSL_OPEN_CERT,        "Failed to open SSL certificate file"},
    {HCNSE_ERR_SSL_READ_CERT,        "Failed to read from SSL certificate file"},
    {HCNSE_ERR_SSL_OPEN_CERTKEY,     "Failed to open SSL certificate key file"},
    {HCNSE_ERR_SSL_READ_CERTKEY,     "Failed to read from SSL certificate key file"},

    /* HCNSE_ERR_PROC domain */
    {HCNSE_ERR_PROC_DAEMON,          "Failed to create server process"},
    {HCNSE_ERR_PROC_WORKDIR,         "Failed to set process workdir"},

    {HCNSE_FAILED,                   "Default internal error"},
    /* End of error list */
    {HCNSE_ERR_END_LIST,             "Undefined error code"}
};


size_t hcnse_strerror_r(hcnse_err_t errcode, char *buf, size_t bufsize)
{
    size_t len;
    size_t n;
    if (errcode == HCNSE_OK) {
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
                error_list[i].code == HCNSE_ERR_END_LIST) {
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
        else {
            continue;
        }
    }
    return n;
}

const char *hcnse_strerror(hcnse_err_t errcode)
{
    if (errcode == HCNSE_OK) {
        return message_ok;
    }
    for (size_t i = 0; ; i++) {
        if (error_list[i].code == errcode ||
                error_list[i].code == HCNSE_ERR_END_LIST) {
            return error_list[i].message;
        }
        else {
            continue;
        }
    }
    return NULL;
}
