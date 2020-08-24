#ifndef INCLUDED_HCNSE_PORTABLE_WIN32_H
#define INCLUDED_HCNSE_PORTABLE_WIN32_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

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

#if (HCNSE_HAVE_OPENSSL)
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#endif

typedef HANDLE                         hcnse_fd_t;
typedef BY_HANDLE_FILE_INFORMATION     hcnse_file_stat_t;
typedef SOCKET                         hcnse_socket_t;
typedef socklen_t                      hcnse_socklen_t;
typedef DWORD                          hcnse_tid_t;
typedef HANDLE                         hcnse_thread_handle_t;
typedef DWORD                          hcnse_thread_value_t;
typedef SYSTEMTIME                     hcnse_tm_t;

#endif /* INCLUDED_HCNSE_PORTABLE_WIN32_H */
