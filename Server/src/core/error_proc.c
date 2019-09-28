#include <stddef.h>
#include <string.h>

#include "error_proc.h"


static const struct {
    status_t code;
    const char *message;
} ErrorList[] = {
    /* CONF_ERROR domain */
    {CONF_INIT_ERROR,              "Error of initialization config."},
    {CONF_OPEN_ERROR,              "Error while open config file."},
    {CONF_OVERFLOW_ERROR,          "Parser section overflow."},
    {CONF_REGEX_ERROR,             "Error of compiling regular expressions."},
    {CONF_SUBSTR_ERROR,            "Error of pcre substring list."},
    {CONF_SYNTAX_ERROR,            "Error of syntax configure file."},

    /* NETWORK_ERROR domain */
    {INIT_SOCKET_ERROR,            "Error of initialization new socket."},
    {ADDR_ERROR,                   "Host IP address is not correct."},
    {BIND_ERROR,                   "Error of binding."},
    {SETSOCKOPT_ERROR,             "Error of set options on sockets"},
    {INIT_LISTEN_ERROR,            "Error of initialization listening socket."},

    /* MEMORY_ERROR domain */
    {ALLOC_MEM_ERROR,              "Error of allocate memory."},
    {INIT_POOL_ERROR,              "Error of initialization new memory pool."},
    {NULL_ADDRESS_ERROR,           "Error of null address."},

    /* SYSLOG_ERROR domain */
    {MAX_LOGSIZE_ERROR,             "Size of log file is too small."},

    /* FILESYSTEM_ERROR domain */
    {OPEN_FILE_ERROR,               "Error of opening file."},
    {WRITE_FILE_ERROR,              "Error of writing file."},

    /* End of error list */
    {SUCCESS,                      "SUCCESS"},
    {OK,                           "OK"},
    {0,                            "Undefined error code."}
};


void cpystrerror(status_t statcode, char *buf, size_t bufsize) {
    size_t len;
    for (int i = 0; ; i++) {           
        if (!ErrorList[i].code || ErrorList[i].code == statcode) {
            len = strlen(ErrorList[i].message);
            if (len > bufsize) {
                memmove(buf, ErrorList[i].message, bufsize);
            }
            else {
                memmove(buf, ErrorList[i].message, len);
            }
            break;
        }
    }
}


const char *set_strerror(status_t statcode) {
    for (int i = 0; ; i++) {      
        if (!ErrorList[i].code || ErrorList[i].code == statcode) {
            return ErrorList[i].message;
        }
    }
}
