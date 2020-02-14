#include <stdio.h>
#include <stdlib.h>

#include "syshead.h"
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
err_t process_daemon_init(void)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        return ERR_PROC_DAEMON;
    }
    /* Parent terminates */
    if (pid) {
        exit(0);
    }
    /* Become session leader */
    if (setsid() < 0) { 
        return ERR_PROC_DAEMON;
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return ERR_PROC_DAEMON;
    }
    /* 1th child terminates */
    if (pid) {
        exit(0);
    }

    /* Close off file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Redirect stdin, stdout, and stderr to /dev/null */
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    return OK;
}

err_t process_set_workdir(const char *workdir)
{
    if ((chdir(workdir)) < 0) {
        return ERR_PROC_WORKDIR;
    }
    return OK;
}

// err_t process_set_priority(int8_t prio) {}



#elif (SYSTEM_WINDOWS)
err_t process_daemon_init(void)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (FreeConsole() == 0) {
        return ERR_PROC_DAEMON;
    }

    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);

    /*
     * TODO: How to redirect stdin, stdout, and stderr to /dev/null on Windows
     */

    return OK;
}

err_t process_set_workdir(const char *workdir)
{
    if (!SetCurrentDirectory(workdir)) {
        return ERR_PROC_WORKDIR;
    }
    return OK;
}

// err_t process_set_priority(int8_t prio) {}

#endif
