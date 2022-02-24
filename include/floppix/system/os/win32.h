#ifndef FLOPPIX_SYSTEM_OS_WIN32_H
#define FLOPPIX_SYSTEM_OS_WIN32_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <process.h>
#include <io.h>

#if (FPX_HAVE_OPENSSL)
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#endif

typedef HANDLE fpx_fd_t;
typedef BY_HANDLE_FILE_INFORMATION fpx_file_stat_t;
typedef SOCKET fpx_socket_t;
typedef socklen_t fpx_socklen_t;
typedef DWORD fpx_tid_t;
typedef HANDLE fpx_thread_handle_t;
typedef DWORD fpx_thread_value_t;
typedef SYSTEMTIME fpx_tm_t;

#endif /* FLOPPIX_SYSTEM_OS_WIN32_H */
