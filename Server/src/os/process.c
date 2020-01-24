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
xxx_err_t init_daemon(void)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        return XXX_FAILED;
    }
    /* Parent terminates */
    if (pid) {
        exit(0);
    }
    /* Become session leader */
    if (setsid() < 0) { 
        return XXX_FAILED;
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return XXX_FAILED;
    }
    /* 1th child terminates */
    if (pid) {
        exit(0);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        return XXX_FAILED;
    }

    /* Close off file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Redirect stdin, stdout, and stderr to /dev/null */
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    return XXX_OK;
}

#elif (SYSTEM_WINDOWS)
xxx_err_t init_daemon(void)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);

    /*
     * TODO: How to redirect stdin, stdout, and stderr to /dev/null on Windows
     */

    return XXX_OK;
}
#endif
