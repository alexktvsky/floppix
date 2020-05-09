#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX && HCNSE_HAVE_DLOPEN)

const char *
hcnse_dlerror(void)
{
    char *err;

    err = (char *) dlerror();
    if (err == NULL) {
        return "";
    }
    return err;
}

#elif (HCNSE_WIN32)

const char *
hcnse_dlerror(void)
{
    return hcnse_strerror(hcnse_get_errno());
}

#endif
