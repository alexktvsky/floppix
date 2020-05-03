#ifndef INCLUDED_HCNSE_PORTABLE_WINDOWS_H
#define INCLUDED_HCNSE_PORTABLE_WINDOWS_H

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

#endif /* INCLUDED_HCNSE_PORTABLE_WINDOWS_H */
