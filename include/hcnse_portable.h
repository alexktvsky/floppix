#ifndef INCLUDED_HCNSE_PORTABLE_H
#define INCLUDED_HCNSE_PORTABLE_H

#include "hcnse_config.h"

#if (HCNSE_LINUX)
#include "hcnse_portable_linux.h"

#elif (HCNSE_FREEBSD)
#include "hcnse_portable_freebsd.h"

#elif (HCNSE_DARWIN)
#include "hcnse_portable_darwin.h"

#elif (HCNSE_SOLARIS)
#include "hcnse_portable_solaris.h"

#elif (HCNSE_WIN32)
#include "hcnse_portable_win32.h"

#endif

typedef int                            hcnse_err_t;
typedef intptr_t                       hcnse_int_t;
typedef uintptr_t                      hcnse_uint_t;
typedef size_t                         hcnse_size_t;
typedef ssize_t                        hcnse_ssize_t;
typedef intptr_t                       hcnse_off_t;
typedef uint32_t                       hcnse_bitfield_t;
typedef uintptr_t                      hcnse_msec_t;
typedef uintptr_t                      hcnse_file_access_t;
typedef uintptr_t                      hcnse_file_type_t;
typedef struct hcnse_file_s            hcnse_file_t;
typedef struct hcnse_dir_s             hcnse_dir_t;
typedef struct hcnse_glob_s            hcnse_glob_t;
typedef struct hcnse_file_info_s       hcnse_file_info_t;
typedef struct hcnse_thread_s          hcnse_thread_t;
typedef struct hcnse_mutex_s           hcnse_mutex_t;
typedef struct hcnse_semaphore_s       hcnse_semaphore_t;

#include "hcnse_errno.h"
#include "hcnse_time.h"
#include "hcnse_memory.h"
#include "hcnse_limits.h"
#include "hcnse_socket.h"
#include "hcnse_sockopt.h"
#include "hcnse_sockaddr.h"
#include "hcnse_filesys.h"
#include "hcnse_process.h"
#include "hcnse_signal.h"
#include "hcnse_thread.h"
#include "hcnse_mutex.h"
#include "hcnse_semaphore.h"
#include "hcnse_cpuinfo.h"
#include "hcnse_dso.h"

#define hcnse_bit_set(x, bit)     (x |= bit)
#define hcnse_bit_unset(x, bit)   (x &= ~(bit))
#define hcnse_bit_is_set(x, bit)  (x & bit)


#endif /* INCLUDED_HCNSE_PORTABLE_H */
