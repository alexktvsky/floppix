#ifndef INCLUDED_OS_PROCESS_H
#define INCLUDED_OS_PROCESS_H

#include "server/errors.h"

hcnse_err_t hcnse_process_daemon_init(void);
hcnse_err_t hcnse_process_set_workdir(const char *workdir);
// hcnse_err_t hcnse_process_set_priority(int8_t);

#endif /* INCLUDED_OS_PROCESS_H */
