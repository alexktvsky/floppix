#ifndef INCLUDED_HCNSE_PROCESS_H
#define INCLUDED_HCNSE_PROCESS_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

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

#endif /* INCLUDED_HCNSE_PROCESS_H */
