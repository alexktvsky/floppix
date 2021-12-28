#include "hcnse.system.dso.h"
#include "hcnse.util.string.h"
#include "hcnse.system.memory.h"

#if (HCNSE_POSIX)

const char *
hcnse_dlerror(char *buf, size_t bufsize)
{
    char *str;
    size_t len;

    str = (char *) dlerror();
    if (str == NULL) {
        str = "";
    }

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

const char *
hcnse_dlerror(char *buf, size_t bufsize)
{
    return hcnse_strerror(hcnse_get_errno(), buf, bufsize);
}

#endif
