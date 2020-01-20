#ifndef XXX_PLATFORM_H
#define XXX_PLATFORM_H

#if defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_NAME "Linux"
#define SYSTEM_LINUX   1
#define SYSTEM_FREEBSD 0
#define SYSTEM_SOLARIS 0
#define SYSTEM_WIN32   0
#define SYSTEM_WIN64   0
#define SYSTEM_WINDOWS 0

#elif defined(__FreeBSD__)
#define PLATFORM_NAME "FreeBSD"
#define SYSTEM_LINUX   0
#define SYSTEM_FREEBSD 1
#define SYSTEM_SOLARIS 0
#define SYSTEM_WIN32   0
#define SYSTEM_WIN64   0
#define SYSTEM_WINDOWS 0

#elif defined(__sun) && defined(__SVR4)
#define PLATFORM_NAME "Solaris"
#define SYSTEM_LINUX   0
#define SYSTEM_FREEBSD 0
#define SYSTEM_SOLARIS 1
#define SYSTEM_WIN32   0
#define SYSTEM_WIN64   0
#define SYSTEM_WINDOWS 0

#elif defined(__WIN32__)
#define PLATFORM_NAME "Win32"
#define SYSTEM_LINUX   0
#define SYSTEM_FREEBSD 0
#define SYSTEM_SOLARIS 0
#define SYSTEM_WIN32   1
#define SYSTEM_WIN64   0
#define SYSTEM_WINDOWS 1

#elif defined(__WIN64__)
#define PLATFORM_NAME "Win64"
#define SYSTEM_LINUX   0
#define SYSTEM_FREEBSD 0
#define SYSTEM_SOLARIS 0
#define SYSTEM_WIN32   0
#define SYSTEM_WIN64   1
#define SYSTEM_WINDOWS 1

#else
#error Unsupported operating system!
#endif

#endif /* XXX_PLATFORM_H */
