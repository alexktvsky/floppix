#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "syshead.h"
#include "sys_errno.h"


#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
const char *sys_strerror(sys_err_t err) {
    return strerror(err);
}

size_t sys_cpystrerror(sys_err_t err, char *buf, size_t bufsize) {
    char *strerr = strerror(err);
    size_t len = strlen(strerr);
    size_t n;
    if (len > bufsize) {
        memmove(buf, strerr, bufsize);
        n = bufsize;
    }
    else {
        memmove(buf, strerr, len);
        n = len;
    }
    return n;
}


#elif (SYSTEM_WINDOWS)
const char *sys_strerror(sys_err_t err) {
    static _Thread_local char buf[1024];
    sys_cpystrerror(err, buf, sizeof(buf));
    return buf;
}

size_t sys_cpystrerror(sys_err_t err, char *buf, size_t bufsize) {

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
