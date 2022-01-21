#include "floppix/system/process.h"
#include "floppix/core/log.h"

#if (FPX_POSIX)

fpx_err_t
fpx_process_become_daemon(void)
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        return fpx_get_errno();
    }
    /* Parent terminates */
    if (pid) {
        exit(0);
    }
    /* Become session leader */
    if (setsid() < 0) {
        return fpx_get_errno();
    }

    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        return fpx_get_errno();
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

    return FPX_OK;
}

fpx_err_t
fpx_process_set_workdir(const char *workdir)
{
    if ((chdir(workdir)) < 0) {
        return fpx_get_errno();
    }
    return FPX_OK;
}

fpx_err_t
fpx_process_set_user(const char *user)
{
    struct passwd *pwd;
    fpx_int_t uid;
    fpx_err_t err;

    /* Not root, OK */
    if (geteuid() != 0) {
        return FPX_OK;
    }

    pwd = getpwnam(user);
    if (!pwd) {
        err = FPX_FAILED;
        fpx_log_error1(FPX_LOG_ERROR, err, "getpwnam(\"%s\") failed", user);
        return err;
    }

    uid = pwd->pw_uid;

    if (setuid(uid) == -1) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "setuid(%d) failed", uid);
        return err;
    }

    return FPX_OK;
}

fpx_err_t
fpx_process_set_group(const char *group)
{
    struct group *grp;
    fpx_int_t gid;
    fpx_err_t err;

    /* Not root, OK */
    if (getegid() != 0) {
        return FPX_OK;
    }

    grp = getgrnam(group);
    if (!grp) {
        err = FPX_FAILED;
        fpx_log_error1(FPX_LOG_ERROR, err, "getgrnam(\"%s\") failed", group);
        return err;
    }

    gid = grp->gr_gid;

    if (setgid(gid) == -1) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "setgid(%d) failed", gid);
        return err;
    }

    return FPX_OK;
}

#elif (FPX_WIN32)

fpx_err_t
fpx_process_become_daemon(void)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (FreeConsole() == 0) {
        return fpx_get_errno();
    }

    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);

    /*
     * TODO: How to redirect stdin, stdout, and stderr to /dev/null on Windows
     */

    return FPX_OK;
}

fpx_err_t
fpx_process_set_workdir(const char *workdir)
{
    if (!SetCurrentDirectory(workdir)) {
        return fpx_get_errno();
    }
    return FPX_OK;
}

fpx_err_t
fpx_process_set_user(const char *user)
{
    (void) user;

    fpx_log_error1(FPX_LOG_WARN, FPX_OK,
        "\"user\" is not supported on this platform");
    return FPX_OK;
}

fpx_err_t
fpx_process_set_group(const char *group)
{
    (void) group;

    fpx_log_error1(FPX_LOG_WARN, FPX_OK,
        "\"group\" is not supported on this platform");
    return FPX_OK;
}

#endif
