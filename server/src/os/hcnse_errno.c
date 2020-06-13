#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_ERRSTR_LEN    1024
#define HCNSE_ERR_END_LIST  0


static const char *message_ok = "OK";

static const struct {
    hcnse_err_t code;
    const char *message;
} error_list[] = {
    {HCNSE_FAILED,                   "Internal server error"},
    {HCNSE_BUSY,                     "Resource is busy"},
    {HCNSE_DONE,                     "Operation is done"},
    {HCNSE_ABORT,                    "Operation aborted"},
    {HCNSE_DECLINED,                 "Operation declined"},
    {HCNSE_NOT_FOUND,                "Resource not found"},

    /* {HCNSE_ERR_CONF domain */
    {HCNSE_ERR_CONF_SYNTAX,          "Error of configuration file syntax"},

    /* HCNSE_ERR_SSL domain */
    {HCNSE_ERR_SSL_INIT,             "Failed to initialize SSL library"},

    /* End of error list */
    {HCNSE_ERR_END_LIST,             "Undefined error code"}
};


#if (HCNSE_POSIX)
const char *
hcnse_errno_strerror(hcnse_errno_t err)
{
    return strerror(err);
}

size_t
hcnse_errno_strerror_r(hcnse_errno_t err, char *buf, size_t bufsize)
{
    char *strerr = strerror(err);
    size_t len = hcnse_strlen(strerr);
    size_t n;
    if (len > bufsize) {
        hcnse_memmove(buf, strerr, bufsize);
        n = bufsize;
    }
    else {
        hcnse_memmove(buf, strerr, len);
        n = len;
    }
    return n;
}


#elif (HCNSE_WIN32)
const char *
hcnse_errno_strerror(hcnse_errno_t err)
{
    static _Thread_local char buf[HCNSE_ERRSTR_LEN];
    hcnse_errno_strerror_r(err, buf, sizeof(buf));
    return buf;
}

size_t
hcnse_errno_strerror_r(hcnse_errno_t err, char *buf, size_t bufsize)
{

    static DWORD lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    DWORD len;

    if (bufsize == 0) {
        return 0;
    }

    len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL, err, lang, buf, bufsize, NULL);

    if (len == 0 && lang) {

        /*
         * Try to use English messages first and fallback to a language,
         * based on locale: non-English Windows have no English messages
         * at all. This way allows to use English messages at least on
         * Windows with MUI.
         */

        lang = 0;

        len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL, err, lang, (char *) buf, bufsize, NULL);
    }

    if (len == 0) {
        return 0;
    }

    /* remove ".\r\n\0" */
    while (buf[len] == '\0' || buf[len] == '\r'
                            || buf[len] == '\n' || buf[len] == '.') {
        buf[len] = '\0';
        len -= 1;
    }
    return len;
}
#endif

size_t
hcnse_strerror_r(hcnse_err_t errcode, char *buf, size_t bufsize)
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

const char *
hcnse_strerror(hcnse_err_t errcode)
{
    if (errcode == HCNSE_OK) {
        return message_ok;
    }

    if (errcode > 0 && errcode < HCNSE_ERROR_DOMAIN_BASE) {
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
