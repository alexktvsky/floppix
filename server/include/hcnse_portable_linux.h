#ifndef INCLUDED_HCNSE_PORTABLE_LINUX_H
#define INCLUDED_HCNSE_PORTABLE_LINUX_H

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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

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

#endif /* INCLUDED_HCNSE_PORTABLE_LINUX_H */
