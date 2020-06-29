#ifndef INCLUDED_HCNSE_DSO_H
#define INCLUDED_HCNSE_DSO_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_POSIX)

#define hcnse_dlopen(path)           dlopen((char *) path, RTLD_NOW|RTLD_GLOBAL)
#define hcnse_dlsym(handle, symbol)  dlsym(handle, symbol)
#define hcnse_dlclose(handle)        dlclose(handle)

#if (HCNSE_HAVE_DLOPEN)
const char *hcnse_dlerror(char *buf, size_t bufsize);
#endif


#elif (HCNSE_WIN32)

#define hcnse_dlopen(path)           LoadLibrary((char *) path)
#define hcnse_dlsym(handle, symbol)  (void *) GetProcAddress(handle, symbol)
#define hcnse_dlclose(handle)        (FreeLibrary(handle) ? 0 : -1)

const char *hcnse_dlerror(char *buf, size_t bufsize);

#endif

#endif /* INCLUDED_HCNSE_DSO_H */
