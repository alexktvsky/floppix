#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "errors.h"

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#elif (SYSTEM_WINDOWS)
#include <windows.h>
#include <io.h>
#endif


#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
status_t init_daemon(void)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        return XXX_FAILED;
    }
    else if (pid) {
        exit(0);
    }

    /* 1th child continues... */

    /* Become session leader */
    if (setsid() < 0) { 
        return XXX_FAILED;
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return XXX_FAILED;
    }

    /* 1th child terminates */
    else if (pid) {
        exit(0);
    }

    /* 2th child continues... */

    chdir("/");

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return XXX_OK;
}

#elif (SYSTEM_WINDOWS)
status_t init_daemon(void)
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);

    return XXX_OK;
}
#endif /* (SYSTEM_WINDOWS) */
