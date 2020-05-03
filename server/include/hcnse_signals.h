#ifndef INCLUDED_HCNSE_SIGNALS_H
#define INCLUDED_HCNSE_SIGNALS_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
hcnse_err_t hcnse_send_wakeup_signal1(pid_t pid);
#endif

/* On UNIX-like systems it send SIGALRM */
hcnse_err_t hcnse_send_wakeup_signal(hcnse_thread_t *tid);
hcnse_err_t hcnse_wait_wakeup_signal(hcnse_msec_t ms);

#endif /* INCLUDED_HCNSE_SIGNALS_H */
