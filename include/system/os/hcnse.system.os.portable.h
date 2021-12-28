#ifndef HCNSE_SYSTEM_OS_PORTABLE_H
#define HCNSE_SYSTEM_OS_PORTABLE_H

#include "hcnse.system.os.config.h"

#if (HCNSE_LINUX)
#include "hcnse.system.os.linux.h"

#elif (HCNSE_FREEBSD)
#include "hcnse.system.os.freebsd.h"

#elif (HCNSE_DARWIN)
#include "hcnse.system.os.darwin.h"

#elif (HCNSE_SOLARIS)
#include "hcnse.system.os.solaris.h"

#elif (HCNSE_WIN32)
#include "hcnse.system.os.win32.h"

#endif

typedef intptr_t                       hcnse_int_t;
typedef uintptr_t                      hcnse_uint_t;
typedef size_t                         hcnse_size_t;
typedef ssize_t                        hcnse_ssize_t;
typedef intptr_t                       hcnse_off_t;


#endif /* HCNSE_SYSTEM_OS_PORTABLE_H */
