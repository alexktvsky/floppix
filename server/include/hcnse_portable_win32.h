#ifndef INCLUDED_HCNSE_PORTABLE_WIN32_H
#define INCLUDED_HCNSE_PORTABLE_WIN32_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <process.h>
#include <io.h>


#if (HCNSE_HAVE_PCRE)
#define PCRE_STATIC
#include <pcre.h>
#endif

typedef int                            hcnse_errno_t;
typedef HANDLE                         hcnse_fd_t;
typedef SOCKET                         hcnse_socket_t;
typedef DWORD                          hcnse_thread_value_t;
typedef SYSTEMTIME                     hcnse_tm_t;

typedef uintptr_t                      hcnse_msec_t;
typedef struct hcnse_file_s            hcnse_file_t;
typedef struct hcnse_thread_s          hcnse_thread_t;
typedef struct hcnse_mutex_s           hcnse_mutex_t;
typedef struct hcnse_semaphore_s       hcnse_semaphore_t;

#endif /* INCLUDED_HCNSE_PORTABLE_WIN32_H */
