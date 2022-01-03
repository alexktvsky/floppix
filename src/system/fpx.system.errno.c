#include "fpx.system.errno.h"
#include "fpx.system.memory.h"
#include "fpx.util.string.h"

#define FPX_ERR_UNKNOW 2147483647

static const struct {
    fpx_err_t code;
    const char *message;
} error_list[] = {
    {FPX_OK,                    "OK"                                },
    {FPX_FAILED,                "Internal server error"             },
    {FPX_BUSY,                  "Resource is busy"                  },
    {FPX_DONE,                  "Operation is done"                 },
    {FPX_ABORT,                 "Operation aborted"                 },
    {FPX_DECLINED,              "Operation declined"                },
    {FPX_NOT_FOUND,             "Resource not found"                },

 /* FPX_ERR_CONF domain */
    {FPX_ERR_CONFIG_SYNTAX,     "Error of configuration file syntax"},

 /* FPX_ERR_FILESYS domain */
    {FPX_ERR_FILESYS_ABS_PATH,  "Specified path is not absolute"    },
    {FPX_ERR_FILESYS_LONG_PATH, "Result path is too long"           },

 /* FPX_ERR_SSL domain */
    {FPX_ERR_SSL_INIT,          "Failed to initialize SSL library"  },

 /* End of error list */
    {FPX_ERR_UNKNOW,            "Unknown error code"                }
};

#if (FPX_POSIX)

static const char *
fpx_os_strerror(fpx_err_t err, char *buf, fpx_size_t bufsize)
{
    char *str;
    fpx_size_t len;

    str = strerror(err);
    len = fpx_strlen(str);

    if (len < bufsize) {
        fpx_memmove(buf, str, len);
        buf[len] = '\0';
    }
    else {
        fpx_memmove(buf, str, bufsize);
        buf[bufsize - 1] = '\0';
    }
    return buf;
}

#elif (FPX_WIN32)

static const char *
fpx_os_strerror(fpx_err_t err, char *buf, fpx_size_t bufsize)
{
    DWORD lang, len;

    lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

    if (bufsize == 0) {
        return NULL;
    }

    len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, lang, buf,
        bufsize, NULL);

    if (len == 0 && lang) {

        /*
         * Try to use English messages first and fallback to a language,
         * based on locale: non-English Windows have no English messages
         * at all. This way allows to use English messages at least on
         * Windows with MUI.
         */

        lang = 0;

        len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, lang,
            (char *) buf, bufsize, NULL);
    }

    if (len == 0) {
        return NULL;
    }

    /* remove ".\r\n\0" */
    while (buf[len] == '\0' || buf[len] == '\r' || buf[len] == '\n'
        || buf[len] == '.')
    {
        buf[len] = '\0';
        len -= 1;
    }
    return buf;
}
#endif

const char *
fpx_strerror(fpx_err_t err, char *buf, fpx_size_t bufsize)
{
    const char *str;
    fpx_size_t len;
    fpx_uint_t i;

    if (err > 0 && err < FPX_ERROR_DOMAIN_BASE) {
        str = fpx_os_strerror(err, buf, bufsize);
        if (!str) {
            return fpx_strerror(FPX_ERR_UNKNOW, buf, bufsize);
        }

        return str;
    }

    for (i = 0;; ++i) {
        if (error_list[i].code == err || error_list[i].code == FPX_ERR_UNKNOW) {
            str = error_list[i].message;
            len = fpx_strlen(str);
            if (len < bufsize) {
                fpx_memmove(buf, str, len);
                buf[len] = '\0';
            }
            else {
                fpx_memmove(buf, str, bufsize);
                buf[bufsize - 1] = '\0';
            }
            break;
        }
        else {
            continue;
        }
    }

    return buf;
}
