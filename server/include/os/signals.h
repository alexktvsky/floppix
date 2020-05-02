#ifndef INCLUDED_SIGNALS_H
#define INCLUDED_SIGNALS_H

#include <signal.h>

#include "os/syshead.h"
#include "os/threads.h"
#include "os/time.h"
#include "server/errors.h"

#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
hcnse_err_t hcnse_send_wakeup_signal1(pid_t pid);
#endif

/* On UNIX-like systems it send SIGALRM */
hcnse_err_t hcnse_send_wakeup_signal(hcnse_thread_t *tid);
hcnse_err_t hcnse_wait_wakeup_signal(hcnse_msec_t ms);

#endif /* INCLUDED_SIGNALS_H */
