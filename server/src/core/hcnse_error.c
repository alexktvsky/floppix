#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_ERR_END_LIST 0


static const char *message_ok = "OK";

static const struct {
    hcnse_err_t code;
    const char *message;
} error_list[] = {
    /* HCNSE_ERR_CONF domain */
    {HCNSE_ERR_CONF_REGEX,           "Failed to compile regular expressions"},

    /* HCNSE_ERR_LOG domain */
    {HCNSE_ERR_LOG_BIG,              "Log file already exist and too bigger than limit"},

    /* HCNSE_ERR_NET domain */
    {HCNSE_ERR_NET_GAI,              "Failed to translate network address"},
    {HCNSE_ERR_NET_GSN,              "Failed to get address to which the socket is bound"},
    {HCNSE_ERR_NET_GPN,              "Failed to get address of the peer connected to the socket"},

    /* HCNSE_ERR_SSL domain */
    {HCNSE_ERR_SSL_INIT,             "Failed to initialize SSL library"},

    {HCNSE_FAILED,                   "Default internal error"},
    /* End of error list */
    {HCNSE_ERR_END_LIST,             "Undefined error code"}
};


size_t hcnse_strerror_r(hcnse_err_t errcode, char *buf, size_t bufsize)
{
    size_t n;

    if (errcode == HCNSE_OK) {
        n = hcnse_strlen(message_ok);
        if (n > bufsize) {
            n = bufsize;
        }
        hcnse_memmove(buf, message_ok, n);
        return n;
    }

    if (errcode < HCNSE_ERROR_DOMAIN_BASE) {
        return hcnse_errno_strerror_r(errcode, buf, bufsize);
    }

    for (size_t i = 0; ; i++) {
        if (error_list[i].code == errcode ||
                error_list[i].code == HCNSE_ERR_END_LIST) {
            n = hcnse_strlen(error_list[i].message);
            if (n > bufsize) {
                n = bufsize;
            }
            hcnse_memmove(buf, error_list[i].message, n);
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

    if (errcode < HCNSE_ERROR_DOMAIN_BASE) {
        return hcnse_errno_strerror(errcode);
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
