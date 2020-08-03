#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)
hcnse_err_t
hcnse_process_daemon_init(void)
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        return hcnse_get_errno();
    }
    /* Parent terminates */
    if (pid) {
        exit(0);
    }
    /* Become session leader */
    if (setsid() < 0) { 
        return hcnse_get_errno();
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return hcnse_get_errno();
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

    return HCNSE_OK;
}

hcnse_err_t
hcnse_process_set_workdir(const char *workdir)
{
    if ((chdir(workdir)) < 0) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

#if 0
hcnse_err_t
hcnse_process_set_priority(hcnse_int_t prio)
{
}
#endif

#elif (HCNSE_WIN32)
hcnse_err_t
hcnse_process_daemon_init(void)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (FreeConsole() == 0) {
        return hcnse_get_errno();
    }

    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);

    /*
     * TODO: How to redirect stdin, stdout, and stderr to /dev/null on Windows
     */

    return HCNSE_OK;
}

hcnse_err_t
hcnse_process_set_workdir(const char *workdir)
{
    if (!SetCurrentDirectory(workdir)) {
        return hcnse_get_errno();
    }
    return HCNSE_OK;
}

#endif
