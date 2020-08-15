#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_ERR_UNKNOW  2147483647

static const struct {
    hcnse_err_t code;
    const char *message;
} error_list[] = {
    {HCNSE_OK,                       "OK"},
    {HCNSE_FAILED,                   "Internal server error"},
    {HCNSE_BUSY,                     "Resource is busy"},
    {HCNSE_DONE,                     "Operation is done"},
    {HCNSE_ABORT,                    "Operation aborted"},
    {HCNSE_DECLINED,                 "Operation declined"},
    {HCNSE_NOT_FOUND,                "Resource not found"},

    /* HCNSE_ERR_CONF domain */
    {HCNSE_ERR_CONFIG_SYNTAX,        "Error of configuration file syntax"},

    /* HCNSE_ERR_SSL domain */
    {HCNSE_ERR_SSL_INIT,             "Failed to initialize SSL library"},

    /* End of error list */
    {HCNSE_ERR_UNKNOW,               "Unknown error code"}
};


#if (HCNSE_POSIX)

static const char *
hcnse_os_strerror(hcnse_err_t err, char *buf, size_t bufsize)
{
    char *str;
    size_t len;

    str = strerror(err);
    len = hcnse_strlen(str);

    if (len < bufsize) {
        hcnse_memmove(buf, str, len);
        buf[len] = '\0';
    }
    else {
        hcnse_memmove(buf, str, bufsize);
        buf[bufsize-1] = '\0';
    }
    return buf;
}


#elif (HCNSE_WIN32)

static const char *
hcnse_os_strerror(hcnse_err_t err, char *buf, size_t bufsize)
{
    DWORD lang, len;

    lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

    if (bufsize == 0) {
        return NULL;
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
        return NULL;
    }

    /* remove ".\r\n\0" */
    while (buf[len] == '\0' || buf[len] == '\r'
                            || buf[len] == '\n' || buf[len] == '.')
    {
        buf[len] = '\0';
        len -= 1;
    }
    return buf;
}
#endif

const char *
hcnse_strerror(hcnse_err_t err, char *buf, size_t bufsize)
{
    const char *str;
    size_t len;
    hcnse_uint_t i;

    if (err > 0 && err < HCNSE_ERROR_DOMAIN_BASE) {
        str = hcnse_os_strerror(err, buf, bufsize);
        if (!str) {
            return hcnse_strerror(HCNSE_ERR_UNKNOW, buf, bufsize);
        }

        return str;
    }

    for (i = 0; ; i++) {
        if (error_list[i].code == err ||
            error_list[i].code == HCNSE_ERR_UNKNOW)
        {
            str = error_list[i].message;
            len = hcnse_strlen(str);
            if (len < bufsize) {
                hcnse_memmove(buf, str, len);
                buf[len] = '\0';
            }
            else {
                hcnse_memmove(buf, str, bufsize);
                buf[bufsize-1] = '\0';
            }
            break;
        }
        else {
            continue;
        }
    }

    return buf;
}
