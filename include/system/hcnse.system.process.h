#ifndef HCNSE_SYSTEM_PROCESS_H
#define HCNSE_SYSTEM_PROCESS_H

#include "hcnse.system.os.portable.h"
#include "hcnse.system.errno.h"

#define HCNSE_DEFAULT_USER             NULL
#define HCNSE_DEFAULT_GROUP            NULL


#if (HCNSE_POSIX)

#define HCNSE_DEFAULT_WORKDIR          "/"
#define HCNSE_DEFAULT_PRIORITY         0

#elif (HCNSE_WIN32)

#define HCNSE_DEFAULT_WORKDIR          "C:\\"
#define HCNSE_DEFAULT_PRIORITY         0

#endif


hcnse_err_t hcnse_process_become_daemon(void);
hcnse_err_t hcnse_process_set_workdir(const char *workdir);
hcnse_err_t hcnse_process_set_user(const char *user);
hcnse_err_t hcnse_process_set_group(const char *group);

/* hcnse_err_t hcnse_process_set_priority(hcnse_int_t); */

#endif /* HCNSE_SYSTEM_PROCESS_H */
