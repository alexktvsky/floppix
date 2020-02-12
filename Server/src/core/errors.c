#include <stddef.h>
#include <string.h>

#include "errors.h"

#define ERR_UNDEF_CODE 0


static const struct {
    err_t code;
    const char *message;
} error_list[] = {
    /* ERR_CONF domain */
    {ERR_CONF_REGEX,             "Failed to compile regular expressions"},

    /* ERR_NET domain */
    {ERR_NET_SOCKET,             "Failed to initialize socket"},
    {ERR_NET_ADDR,               "IP address is not correct"},
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
    {ERR_MEM_NULL_ADDR,          "A null pointer dereference"},
    {ERR_MEM_ALLOC,              "Failed to allocate memory"},
    {ERR_MEM_INIT_POOL,          "Failed to initialize memory pool"},

    /* ERR_LOG domain */
    {ERR_LOGSIZE,                "Size of log file is too small"},

    /* ERR_FILE domain */
    {ERR_FILE_OPEN,              "Failed to open file"},
    {ERR_FILE_SIZE,              "Failed to determine file size"},
    {ERR_FILE_READ,              "Failed to read data from file"},
    {ERR_FILE_WRITE,             "Failed to write data to file"},

    {ERR_FAILED,                 "Default system error"},
    /* End of error list */
    {ERR_UNDEF_CODE,             "Undefined error code"}
};

static const char *message_ok = "OK";


void cpystrerror(err_t errcode, char *buf, size_t bufsize)
{
    size_t len;
    if (errcode == ERR_OK) {
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
                    error_list[i].code == ERR_UNDEF_CODE) {
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


const char *get_strerror(err_t errcode)
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
