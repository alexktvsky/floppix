#ifndef INCLUDED_PROCESS_H
#define INCLUDED_PROCESS_H

#include "errors.h"

err_t process_daemon_init(void);
err_t process_set_workdir(const char *workdir);
// err_t process_set_priority(int8_t);

#endif /* INCLUDED_PROCESS_H */
