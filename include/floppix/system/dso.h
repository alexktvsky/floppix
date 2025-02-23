#ifndef FLOPPIX_SYSTEM_DSO_H
#define FLOPPIX_SYSTEM_DSO_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"

#if (FPX_POSIX)

#define fpx_dlopen(path)          dlopen((char *) path, RTLD_NOW | RTLD_GLOBAL)
#define fpx_dlsym(handle, symbol) dlsym(handle, symbol)
#define fpx_dlclose(handle)       dlclose(handle)

const char *fpx_dlerror(char *buf, size_t bufsize);

#elif (FPX_WIN32)

#define fpx_dlopen(path)          LoadLibrary((char *) path)
#define fpx_dlsym(handle, symbol) (void *) GetProcAddress(handle, symbol)
#define fpx_dlclose(handle)       (FreeLibrary(handle) ? 0 : -1)

const char *fpx_dlerror(char *buf, size_t bufsize);

#endif

#endif /* FLOPPIX_SYSTEM_DSO_H */
