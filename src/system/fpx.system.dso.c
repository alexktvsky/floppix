#include "fpx.system.dso.h"
#include "fpx.util.string.h"
#include "fpx.system.memory.h"

#if (FPX_POSIX)

const char *
fpx_dlerror(char *buf, fpx_size_t bufsize)
{
    char *str;
    fpx_size_t len;

    str = (char *) dlerror();
    if (str == NULL) {
        str = "";
    }

    len = fpx_strlen(str);

    if (len < bufsize) {
        fpx_memmove(buf, str, len);
        buf[len] = '\0';
    }
    else {
        fpx_memmove(buf, str, bufsize);
        buf[bufsize-1] = '\0';
    }
    return buf;
}

#elif (FPX_WIN32)

const char *
fpx_dlerror(char *buf, fpx_size_t bufsize)
{
    return fpx_strerror(fpx_get_errno(), buf, bufsize);
}

#endif
