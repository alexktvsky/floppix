#ifndef INCLUDED_HCNSE_PROCESS_H
#define INCLUDED_HCNSE_PROCESS_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t hcnse_process_daemon_init(void);
hcnse_err_t hcnse_process_set_workdir(const char *workdir);
// hcnse_err_t hcnse_process_set_priority(int8_t);

#endif /* INCLUDED_HCNSE_PROCESS_H */
