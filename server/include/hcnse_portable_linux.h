#ifndef INCLUDED_HCNSE_PORTABLE_LINUX_H
#define INCLUDED_HCNSE_PORTABLE_LINUX_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>

#if (HCNSE_HAVE_MMAP)
#include <sys/mman.h>
#endif

#if (HCNSE_HAVE_SELECT)
#include <sys/select.h>
#endif

#if (HCNSE_HAVE_EPOLL)
#include <sys/epoll.h>
#endif

#if (HCNSE_HAVE_PCRE)
#include <pcre.h>
#endif

#if (HCNSE_HAVE_OPENSSL)
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#endif

typedef int                            hcnse_fd_t;
typedef int                            hcnse_socket_t;
typedef pid_t                          hcnse_tid_t;
typedef pthread_t                      hcnse_thread_handle_t;
typedef void *                         hcnse_thread_value_t;
typedef struct tm                      hcnse_tm_t;

#endif /* INCLUDED_HCNSE_PORTABLE_LINUX_H */
