#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_file_type_t
hcnse_stat_get_file_type(hcnse_file_stat_t *stat)
{
    hcnse_file_type_t type;
    mode_t mode;

    type = HCNSE_FILE_TYPE_UNKFILE;
    mode = stat->st_mode;

    if (S_ISREG(mode)) {
        type = HCNSE_FILE_TYPE_FILE;
    }
    if (S_ISDIR(mode)) {
        type = HCNSE_FILE_TYPE_DIR;
    }
    if (S_ISCHR(mode)) {
        type = HCNSE_FILE_TYPE_CHR;
    }

#if defined(S_ISBLK)
    if (S_ISBLK(mode)) {
        type = HCNSE_FILE_TYPE_BLK;
    }
#endif

#if defined(S_ISFIFO)
    if (S_ISFIFO(mode)) {
        type = HCNSE_FILE_TYPE_PIPE;
    }
#endif

#if defined(S_ISLNK)
    if (S_ISLNK(mode)) {
        type = HCNSE_FILE_TYPE_LINK;
    }
#endif

#if defined(S_ISLNK)
    if (S_ISSOCK(mode)) {
        type = HCNSE_FILE_TYPE_SOCK;
    }
#endif

    return type;
}

static hcnse_file_access_t
hcnse_stat_get_access(hcnse_file_stat_t *stat)
{
    hcnse_file_access_t access;
    mode_t mode;

    access = 0;
    mode = stat->st_mode;

    if (mode & S_ISUID) {
        access |= HCNSE_FILE_ACCESS_USETID;
    }
    if (mode & S_IRUSR) {
        access |= HCNSE_FILE_ACCESS_UR;
    }
    if (mode & S_IWUSR) {
        access |= HCNSE_FILE_ACCESS_UW;
    }
    if (mode & S_IXUSR) {
        access |= HCNSE_FILE_ACCESS_UX;
    }

    if (mode & S_ISGID) {
        access |= HCNSE_FILE_ACCESS_GSETID;
    }
    if (mode & S_IRGRP) {
        access |= HCNSE_FILE_ACCESS_GR;
    }
    if (mode & S_IWGRP) {
        access |= HCNSE_FILE_ACCESS_GW;
    }
    if (mode & S_IXGRP) {
        access |= HCNSE_FILE_ACCESS_GX;
    }

#if defined(S_ISVTX)
    if (mode & S_ISVTX) {
        access |= HCNSE_FILE_ACCESS_STICKY;
    }
#endif

    if (mode & S_IROTH) {
        access |= HCNSE_FILE_ACCESS_OR;
    }
    if (mode & S_IWOTH) {
        access |= HCNSE_FILE_ACCESS_OW;
    }
    if (mode & S_IXOTH) {
        access |= HCNSE_FILE_ACCESS_OX;
    }

    return access;
}

hcnse_err_t
hcnse_file_open(hcnse_file_t *file, const char *path, hcnse_uint_t mode,
    hcnse_uint_t create, hcnse_uint_t access)
{
    hcnse_fd_t fd;
    hcnse_err_t err;

    hcnse_memzero(file, sizeof(hcnse_file_t));

    fd = open(path, mode|create, access);
    if (fd == HCNSE_INVALID_FILE) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "open(\"%s\", %d|%d, %o) failed", path, mode, create, access);
        return err;
    }

    file->fd = fd;
    file->name = (char *) path;
    file->offset = (size_t) 0;

    return HCNSE_OK;
}

ssize_t
hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size,
    hcnse_off_t offset)
{
    ssize_t n;

    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_SET) failed", file->fd, offset);
        return -1;
    }
    n = read(file->fd, buf, size);
    if (n == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "read(%d, %p, %zu) failed", file->fd, buf, size);
        return -1;
    }
    file->offset += n;
    return n;
}

ssize_t
hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size,
    hcnse_off_t offset)
{
    ssize_t n;
    ssize_t written = 0;

    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, %s) failed", file->fd, offset,
            hcnse_value(SEEK_SET));
        return -1;
    }

    for ( ; ; ) {
        n = write(file->fd, buf + written, size);
        if (n == -1) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "write(%d, %p, %zu) failed", file->fd, buf + written, size);
            return -1;
        }
        file->offset += n;
        written += n;
        if ((size_t) n == size) {
            return written;
        }
        size -= n;
    }
}

/* Work even we already read some bytes from file */
ssize_t
hcnse_file_size(hcnse_file_t *file)
{
    hcnse_off_t off;
    size_t size;

    off = lseek(file->fd, 0, SEEK_CUR);
    if (off == (hcnse_off_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_CUR) failed", file->fd, 0);
        return -1;
    }
    size = (size_t) lseek(file->fd, 0, SEEK_END);
    if (size == (size_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_END) failed", file->fd, 0);
        return -1;
    }
    if (lseek(file->fd, off, SEEK_SET) == (hcnse_off_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_SET) failed", file->fd, 0);
        return -1;
    }
    return size;
}

ssize_t
hcnse_read_fd(hcnse_fd_t fd, void *buf, size_t n)
{
    ssize_t rv;

    rv = read(fd, buf, n);
    if (rv == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "read(%d, %p, %zu) failed", fd, buf, n);
        return -1;
    }
    return rv;
}

ssize_t
hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t n)
{
    ssize_t rv;

    rv = write(fd, buf, n);
    if (rv == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "write(%d, %p, %zu) failed", fd, buf, n);
        return -1;
    }
    return rv;
}


hcnse_err_t
hcnse_file_info_by_path(hcnse_file_info_t *info, const char *path)
{
    hcnse_err_t err;

    if (stat(path, &(info->stat)) == -1) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "stat(\"%s\", %p) failed", path, info);
        err = hcnse_get_errno();
        return err;
    }
    info->type = hcnse_stat_get_file_type(&(info->stat));
    info->access = hcnse_stat_get_access(&(info->stat));

    return HCNSE_OK;
}

hcnse_err_t
hcnse_file_info(hcnse_file_info_t *info, hcnse_file_t *file)
{
    hcnse_err_t err;

    if (fstat(file->fd, &(info->stat)) == -1) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "fstat(%d, %p) failed", file->fd, info);
        return err;
    }
    info->type = hcnse_stat_get_file_type(&(info->stat));
    info->access = hcnse_stat_get_access(&(info->stat));

    return HCNSE_OK;
}

void
hcnse_file_close(hcnse_file_t *file)
{
    if (!file) {
        return;
    }

    close(file->fd);
    file->fd = HCNSE_INVALID_FILE;
    file->name = NULL;
    file->offset = (hcnse_off_t) 0;
}

hcnse_err_t
hcnse_dir_open(hcnse_dir_t *dir, const char *path)
{
    hcnse_err_t err;

    hcnse_memzero(dir, sizeof(hcnse_dir_t));

    dir->dir = opendir(path);
    if (dir->dir == NULL) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "opendir(%p, \"%s\") failed", dir, path);
        return err;
    }

    dir->name = path;
    /*
     * Set by memset()
     * dir-type = HCNSE_FILE_TYPE_UNKFILE;
     */

    return HCNSE_OK;
}

#if (HCNSE_HAVE_D_TYPE)

hcnse_err_t
hcnse_dir_read(hcnse_dir_t *dir)
{
    hcnse_err_t err;

    dir->dirent = readdir(dir->dir);
    if (!dir->dirent) {
        err = hcnse_get_errno();
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "readdir(%p) failed for \"%s\"", dir, dir->name);
            return err;
        }
        return HCNSE_DONE;
    }

    dir->type = dir->dirent->d_type;

    return HCNSE_OK;
}
#else

hcnse_err_t
hcnse_dir_read(hcnse_dir_t *dir)
{
    hcnse_file_stat_t fstat;
    char fullpath[HCNSE_MAX_PATH_LEN];
    hcnse_err_t err;

    dir->dirent = readdir(dir->dir);
    if (!dir->dirent) {
        err = hcnse_get_errno();
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "readdir(%p) failed for \"%s\"", dir, dir->name);
            return err;
        }
        return HCNSE_DONE;
    }

    hcnse_file_full_path(fullpath, dir->name, dir->dirent->d_name);

    hcnse_file_stat_by_path(&fstat, fullpath);

    dir->type = hcnse_stat_get_file_type(&fstat);

    return HCNSE_OK;
}
#endif


void
hcnse_dir_close(hcnse_dir_t *dir)
{
    if (!dir) {
        return;
    }
    closedir(dir->dir);
}

const char *
hcnse_dir_current_name(hcnse_dir_t *dir)
{
    return dir->dirent->d_name;
}

hcnse_err_t
hcnse_glob_open(hcnse_glob_t *gl, const char *pattern)
{
    int rv;
    hcnse_err_t err;

    rv = glob(pattern, 0, NULL, &gl->glob);
    if (rv != 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "glob(%s, %p) failed", gl->pattern, &gl->glob);
        return err;
    }

    gl->pattern = pattern;
    gl->last = 0;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_glob_read(hcnse_glob_t *gl, char *res)
{
    size_t n, len;

#ifdef GLOB_NOMATCH
    n = (size_t) gl->glob.gl_pathc;
#else
    n = (size_t) gl->glob.gl_matchc;
#endif

    if (gl->last >= n) {
        return HCNSE_DONE;
    }

    len = strlen(gl->glob.gl_pathv[gl->last]);
    memmove(res, gl->glob.gl_pathv[gl->last], len + 1);
    gl->last += 1;

    return HCNSE_OK;
}

void
hcnse_glob_close(hcnse_glob_t *gl)
{
    globfree(&gl->glob);
}


hcnse_err_t
hcnse_check_absolute_path(const char *path)
{
    if (*path == '/') {
        return HCNSE_OK;
    }

    return HCNSE_ERR_FILESYS_ABS_PATH;
}


ssize_t
hcnse_write_stdout(const char *str)
{
    return hcnse_write_fd(HCNSE_STDOUT, (void *) str, strlen(str));
}

ssize_t
hcnse_write_stderr(const char *str)
{
    return hcnse_write_fd(HCNSE_STDERR, (void *) str, strlen(str));
}

hcnse_file_type_t
hcnse_dir_current_file_type(hcnse_dir_t *dir)
{
    return dir->type;
}

hcnse_err_t
hcnse_dir_current_is_file(hcnse_dir_t *dir)
{
    return (dir->type == HCNSE_FILE_TYPE_FILE);
}

size_t
hcnse_file_full_path(char *buf, const char *path, const char *file)
{
    size_t path_len, file_len, copied;

    path_len = hcnse_strlen(path);
    file_len = hcnse_strlen(file);

    if (hcnse_is_path_separator(path[path_len - 1])) {
        path_len -= 1;
    }

    copied = 0;

    hcnse_memmove(buf + copied, path, path_len);
    copied += path_len;

    hcnse_memmove(buf + copied, HCNSE_PATH_SEPARATOR_STR, 1);
    copied += + 1;

    hcnse_memmove(buf + copied, file, file_len);
    copied += file_len;

    buf[copied] = '\0';

    return copied;
}

hcnse_flag_t
hcnse_is_path_has_wildcard(const char *path)
{
    char *ch;

    ch = (char *) path;

    while (*ch) {
        switch (*ch) {
        case '?':
        case '*':
            return 1;
        }
        ++ch;
    }
    return 0;
}
