#ifndef FLOPPIX_SYSTEM_PROCESS_H
#define FLOPPIX_SYSTEM_PROCESS_H

#include "floppix/system/os/portable.h"
#include "floppix/system/errno.h"

#define FPX_DEFAULT_USER  NULL
#define FPX_DEFAULT_GROUP NULL

#if (FPX_POSIX)

#define FPX_DEFAULT_WORKDIR  "/"
#define FPX_DEFAULT_PRIORITY 0

#elif (FPX_WIN32)

#define FPX_DEFAULT_WORKDIR  "C:\\"
#define FPX_DEFAULT_PRIORITY 0

#endif

fpx_err_t fpx_process_become_daemon(void);
fpx_err_t fpx_process_set_workdir(const char *workdir);
fpx_err_t fpx_process_set_user(const char *user);
fpx_err_t fpx_process_set_group(const char *group);

/* fpx_err_t fpx_process_set_priority(int); */

#endif /* FLOPPIX_SYSTEM_PROCESS_H */
