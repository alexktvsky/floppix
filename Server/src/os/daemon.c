#include <stdlib.h>

#include "platform.h"
#include "error_proc.h"

#if SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#endif
#if SYSTEM_WIN32 || SYSTEM_WIN64
#include <windows.h>
#endif


#if SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS
status_t init_daemon(void)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        return FAILED;
    }

    /* Parent terminates */
    else if (pid) {
        exit(0);
    }

    /* 1th child continues... */

    /* Become session leader */
    if (setsid() < 0) { 
        return FAILED;
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return FAILED;
    }

    /* 1th child terminates */
    else if (pid) {
        exit(0);
    }

    /* 2th child continues... */

    chdir("/");

    /* Close off file descriptors */
/*
    for (int i = 0; i < MAXFD; i++) {
        close(i);
    }
*/

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return OK;
}
#endif /* SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS */


#if SYSTEM_WIN32 || SYSTEM_WIN64
status_t init_daemon(void)
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
    return OK;
}
#endif /* SYSTEM_WIN32 || SYSTEM_WIN64 */
