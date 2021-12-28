#include "hcnse.system.process.h"
#include "hcnse.core.log.h"

#if (HCNSE_POSIX)

hcnse_err_t
hcnse_process_become_daemon(void)
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

hcnse_err_t
hcnse_process_set_user(const char *user)
{
    struct passwd *pwd;
    hcnse_int_t uid;
    hcnse_err_t err;

    /* Not root, OK */
    if (geteuid() != 0) {
        return HCNSE_OK;
    }

    pwd = getpwnam(user);
    if (!pwd) {
        err = HCNSE_FAILED;
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "getpwnam(\"%s\") failed", user);
        return err;
    }

    uid = pwd->pw_uid;

    if (setuid(uid) == -1) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "setuid(%d) failed", uid);
        return err;
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_process_set_group(const char *group)
{
    struct group *grp;
    hcnse_int_t gid;
    hcnse_err_t err;

    /* Not root, OK */
    if (getegid() != 0) {
        return HCNSE_OK;
    }

    grp = getgrnam(group);
    if (!grp) {
        err = HCNSE_FAILED;
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "getgrnam(\"%s\") failed", group);
        return err;
    }

    gid = grp->gr_gid;

    if (setgid(gid) == -1) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err, "setgid(%d) failed", gid);
        return err;
    }

    return HCNSE_OK;
}

#elif (HCNSE_WIN32)

hcnse_err_t
hcnse_process_become_daemon(void)
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

hcnse_err_t
hcnse_process_set_user(const char *user)
{
    (void) user;

    hcnse_log_error1(HCNSE_LOG_WARN, HCNSE_OK,
        "\"user\" is not supported on this platform");
    return HCNSE_OK;
}

hcnse_err_t
hcnse_process_set_group(const char *group)
{
    (void) group;

    hcnse_log_error1(HCNSE_LOG_WARN, HCNSE_OK,
        "\"group\" is not supported on this platform");
    return HCNSE_OK;
}



#endif
