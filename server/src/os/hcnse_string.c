#include "hcnse_portable.h"
#include "hcnse_core.h"


ssize_t hcnse_fprintf(hcnse_fd_t fd, const char *fmt, ...)
{
    va_list args;
    size_t len;
    static char buf[HCNSE_MAX_STRING_SIZE];

    va_start(args, fmt);
    len = hcnse_vsnprintf(buf, HCNSE_MAX_STRING_SIZE, fmt, args);
    va_end(args);

    return hcnse_write_fd(fd, buf, len);
}

