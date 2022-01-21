#ifndef FLOPPIX_SYSTEM_OS_LINUX_H
#define FLOPPIX_SYSTEM_OS_LINUX_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <glob.h>
#include <pthread.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <pwd.h>
#include <grp.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>

#if (FPX_HAVE_MMAP)
#include <sys/mman.h>
#endif

#if (FPX_HAVE_SELECT)
#include <sys/select.h>
#endif

#if (FPX_HAVE_EPOLL)
#include <sys/epoll.h>
#endif

#if (FPX_HAVE_OPENSSL)
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#endif

typedef int fpx_fd_t;
typedef struct stat fpx_file_stat_t;
typedef int fpx_socket_t;
typedef socklen_t fpx_socklen_t;
typedef pid_t fpx_tid_t;
typedef pthread_t fpx_thread_handle_t;
typedef void *fpx_thread_value_t;
typedef struct tm fpx_tm_t;

#endif /* FLOPPIX_SYSTEM_OS_LINUX_H */
