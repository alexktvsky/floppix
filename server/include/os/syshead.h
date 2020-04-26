#ifndef INCLUDED_SYSHEAD_H
#define INCLUDED_SYSHEAD_H

#if defined(__linux__) || defined(__gnu_linux__)
#define HCNSE_SYSTEM_NAME "Linux"
#define HCNSE_LINUX   1
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 0
#define HCNSE_WIN32   0
#define HCNSE_WIN64   0
#define HCNSE_WINDOWS 0

#elif defined(__FreeBSD__)
#define HCNSE_SYSTEM_NAME "FreeBSD"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 1
#define HCNSE_SOLARIS 0
#define HCNSE_WIN32   0
#define HCNSE_WIN64   0
#define HCNSE_WINDOWS 0

#elif defined(__sun) && defined(__SVR4)
#define HCNSE_SYSTEM_NAME "Solaris"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 1
#define HCNSE_WIN32   0
#define HCNSE_WIN64   0
#define HCNSE_WINDOWS 0

#elif defined(__WIN64__)
#define HCNSE_SYSTEM_NAME "Win64"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 0
#define HCNSE_WIN64   1
#define HCNSE_WIN32   0
#define HCNSE_WINDOWS 1

#elif defined(__WIN32__)
#define HCNSE_SYSTEM_NAME "Win32"
#define HCNSE_LINUX   0
#define HCNSE_FREEBSD 0
#define HCNSE_SOLARIS 0
#define HCNSE_WIN64   0
#define HCNSE_WIN32   1
#define HCNSE_WINDOWS 1

#else
#error "Unsupported operating system"
#endif

#define hcnse_system_name() (HCNSE_SYSTEM_NAME)

#endif /* INCLUDED_SYSHEAD_H */
