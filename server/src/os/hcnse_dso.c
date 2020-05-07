#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)

#if (HCNSE_HAVE_DLOPEN)
char *hcnse_dlerror(void)
{
    char *err;

    err = (char *) dlerror();
    if (err == NULL) {
        return "";
    }
    return err;
}
#endif

#elif (HCNSE_WIN32)

char *hcnse_dlerror(void)
{
    return hcnse_strerror(hcnse_get_errno());
}

#endif
