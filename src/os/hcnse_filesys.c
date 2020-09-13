#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_POSIX)

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

#elif (HCNSE_WIN32)

static uint32_t
hcnse_utf8_decode(u_char **p, size_t n)
{
    size_t len;
    uint32_t u, i, valid;

    u = **p;

    if (u >= 0xf0) {
        u &= 0x07;
        valid = 0xffff;
        len = 3;
    }
    else if (u >= 0xe0) {
        u &= 0x0f;
        valid = 0x7ff;
        len = 2;
    }
    else if (u >= 0xc2) {
        u &= 0x1f;
        valid = 0x7f;
        len = 1;
    }
    else {
        (*p)++;
        return 0xffffffff;
    }

    if (n - 1 < len) {
        return 0xfffffffe;
    }

    (*p)++;

    while (len) {
        i = *(*p)++;

        if (i < 0x80) {
            return 0xffffffff;
        }

        u = (u << 6) | (i & 0x3f);

        len--;
    }

    if (u > valid) {
        return u;
    }

    return 0xffffffff;
}

static uint16_t *
hcnse_path_to_wchar(uint16_t *outstr, size_t outlen,
    const uint8_t *instr)
{
    uint8_t *p;
    uint16_t *u, *last;
    uint32_t n;

    p = (uint8_t *) instr;
    u = outstr;
    last = outstr + outlen;

    while (u < last) {

        if (*p < 0x80) {
            *u++ = (uint16_t) *p;

            if (*p == 0) {
                return outstr;
            }

            p++;

            continue;
        }

        if (u + 1 == last) {
            break;
        }

        n = hcnse_utf8_decode(&p, 4);

        if (n > 0x10ffff) {
            return NULL;
        }

        if (n > 0xffff) {
            n -= 0x10000;
            *u++ = (uint16_t) (0xd800 + (n >> 10));
            *u++ = (uint16_t) (0xdc00 + (n & 0x03ff));
            continue;
        }

        *u++ = (uint16_t) n;
    }
    return NULL;
}

static hcnse_file_type_t
hcnse_stat_get_file_type(hcnse_file_stat_t *stat)
{
    hcnse_file_type_t type;

    type = HCNSE_FILE_TYPE_UNKFILE;

    if (stat->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        type = HCNSE_FILE_TYPE_DIR;
    }
    if (!(stat->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        type = HCNSE_FILE_TYPE_FILE;
    }

    return type;
}

hcnse_err_t
hcnse_file_open(hcnse_file_t *file, const char *path, hcnse_uint_t mode,
    hcnse_uint_t create, hcnse_uint_t access)
{
    uint16_t wpath[HCNSE_MAX_PATH_LEN];
    hcnse_fd_t fd;
    hcnse_err_t err;

    (void) access;

    hcnse_memzero(file, sizeof(hcnse_file_t));

    hcnse_path_to_wchar(wpath, sizeof(wpath), (const uint8_t *) path);

    fd = CreateFileW(wpath, mode, 0, NULL, create, 0, NULL);
    if (fd == HCNSE_INVALID_FILE) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "CreateFileW(\"%s\", %d, %d, %p, %d, %d, %p) failed",
            path, mode, 0, NULL, create, 0, NULL);
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
    DWORD n;
    OVERLAPPED ovlp;

    ovlp.Internal = 0;
    ovlp.InternalHigh = 0;
    ovlp.Offset = (DWORD) offset;
    /* ovlp.OffsetHigh = (DWORD) (offset >> 32); */
    ovlp.hEvent = NULL;

    if (ReadFile(file->fd, buf, size, &n, &ovlp) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "ReadFile(%p, %p, %zu, %p, %p) failed",
            file->fd, buf, size, &n, &ovlp);
        return -1;
    }
    file->offset += n;
    return (ssize_t) n;
}

ssize_t
hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size,
    hcnse_off_t offset)
{
    DWORD n;
    OVERLAPPED ovlp;

    ovlp.Internal = 0;
    ovlp.InternalHigh = 0;
    ovlp.Offset = (DWORD) offset;
    /* ovlp.OffsetHigh = (DWORD) (offset >> 32); */
    ovlp.hEvent = NULL;

    if (WriteFile(file->fd, buf, size, &n, &ovlp) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "WriteFile(%p, %p, %zu, %p, %p) failed",
            file->fd, buf, size, &n, &ovlp);
        return -1;
    }

    if (((size_t) n) != size) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED, "((size_t) n) != size");
        return -1;
    }
    file->offset += n;
    return n;
}

#if (HCNSE_PTR_SIZE == 8)

ssize_t
hcnse_file_size(hcnse_file_t *file)
{
    LARGE_INTEGER info;
    ssize_t size;

    if (GetFileSizeEx(file->fd, &info) != 1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "GetFileSizeEx(%p, %p) failed", file->fd, &info);
        return -1;
    }
    size = info.QuadPart;

    return (ssize_t) size;
}
#else

ssize_t
hcnse_file_size(hcnse_file_t *file)
{
    ssize_t size;

    size = (ssize_t) GetFileSize(file->fd, NULL);
    if (size == HCNSE_INVALID_FILE_SIZE) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "GetFileSize(%p, %p) failed", file->fd, NULL);
        return -1;
    }
    return (ssize_t) size;
}
#endif

ssize_t
hcnse_read_fd(hcnse_fd_t fd, void *buf, size_t size)
{
    DWORD n;

    if (ReadFile(fd, buf, size, &n, NULL) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "ReadFile(%p, %p, %zu, %p, %p) failed",
            fd, buf, size, &n, NULL);
        return -1;
    }
    return (ssize_t) n;
}

ssize_t
hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t size)
{
    DWORD n;

    if (WriteFile(fd, buf, size, &n, NULL) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "WriteFile(%p, %p, %zu, %p, %p) failed",
            fd, buf, size, &n, NULL);
        return -1;
    }
    return (ssize_t) n;
}

hcnse_err_t
hcnse_file_stat_by_path(hcnse_file_stat_t *stat, const char *path)
{
    WIN32_FILE_ATTRIBUTE_DATA file_attr;
    uint16_t wpath[HCNSE_MAX_PATH_LEN];
    hcnse_err_t err;

    hcnse_path_to_wchar(wpath, sizeof(wpath), (const uint8_t *) path);

    if (GetFileAttributesExW(wpath, GetFileExInfoStandard, &file_attr) == 0) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "GetFileAttributesExW(\"%s\") failed", path);
        return err;
    }

    stat->dwFileAttributes = file_attr.dwFileAttributes;
    stat->ftCreationTime = file_attr.ftCreationTime;
    stat->ftLastAccessTime = file_attr.ftLastAccessTime;
    stat->ftLastWriteTime = file_attr.ftLastWriteTime;
    stat->nFileSizeHigh = file_attr.nFileSizeHigh;
    stat->nFileSizeLow = file_attr.nFileSizeLow;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_file_info_by_path(hcnse_file_info_t *info, const char *path)
{
    hcnse_err_t

    err = hcnse_file_stat_by_path(&(info->stat), path);
    if (err != HCNSE_OK) {
        return err;
    }

    info->type = hcnse_stat_get_file_type(&(info->stat));
    info->access = 0;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_file_info(hcnse_file_info_t *info, hcnse_file_t *file)
{
    hcnse_err_t err;

    if (GetFileInformationByHandle(file->fd, &(info->stat)) == -1) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "GetFileInformationByHandle(%d) failed", file->fd);
        return err;
    }

    info->type = hcnse_stat_get_file_type(&(info->stat));
    info->access = 0;

    return HCNSE_OK;
}

void
hcnse_file_close(hcnse_file_t *file)
{
    if (!file) {
        return;
    }

    CloseHandle(file->fd);
    file->fd = HCNSE_INVALID_FILE;
    file->name = NULL;
    file->offset = (hcnse_off_t) 0;
}

hcnse_err_t
hcnse_dir_open(hcnse_dir_t *dir, const char *path)
{
    char str[HCNSE_MAX_PATH_LEN + 3];
    size_t len;
    hcnse_err_t err;

    hcnse_memzero(dir, sizeof(hcnse_dir_t));

    len = hcnse_strlen(path);

    if (len >= HCNSE_MAX_PATH_LEN + 3) {
        return E2BIG;
    }

    hcnse_memmove(str, path, len);
    str[len + 0] = '\\';
    str[len + 1] = '*';
    str[len + 2] = '\0';

    dir->dir = FindFirstFile((const char *) str, &(dir->finddata));
    if (dir->dir == INVALID_HANDLE_VALUE) {
        err = hcnse_get_errno();
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "FindFirstFile(\"%s\", %p) failed", str, &(dir->finddata));
            return err;
        }
        return HCNSE_DONE;
    }

    dir->name = path;
    /*
     * Set by memset()
     * dir-type = HCNSE_FILE_TYPE_UNKFILE;
     */

    return HCNSE_OK;
}

hcnse_err_t
hcnse_dir_read(hcnse_dir_t *dir)
{
    hcnse_file_stat_t fstat;
    char fullpath[HCNSE_MAX_PATH_LEN];
    hcnse_err_t err;

    if (FindNextFile(dir->dir, &dir->finddata) == 0) {
        err = hcnse_get_errno();
        if (err != HCNSE_OK && err != ERROR_NO_MORE_FILES) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "FindNextFile(%p) failed for \"%s\"",
                dir->dir, &dir->finddata, dir->name);
            return err;
        }
        return HCNSE_DONE;
    }

    hcnse_file_full_path(fullpath, dir->name, dir->finddata.cFileName);

    hcnse_file_stat_by_path(&fstat, fullpath);

    dir->type = hcnse_stat_get_file_type(&fstat);

    return HCNSE_OK;
}

void
hcnse_dir_close(hcnse_dir_t *dir)
{
    if (!dir) {
        return;
    }
    FindClose(dir->dir);
}

hcnse_err_t
hcnse_glob_open(hcnse_glob_t *gl, const char *pattern)
{
    hcnse_int_t i;
    size_t len;
    hcnse_err_t err;

    gl->dir = FindFirstFile((const char *) pattern, &gl->finddata);
    if (gl->dir == INVALID_HANDLE_VALUE) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "FindFirstFile(%s, %p) failed", pattern, &gl->finddata);
        return HCNSE_FAILED;
    }

    len = 0;

    for (i = hcnse_strlen(pattern); i >= 0; --i) {
        if (pattern[i] == '\\') {
            len = (size_t) i;
            break;
        }
    }

    if (len == 0) {
        return HCNSE_ERR_FILESYS_ABS_PATH;
    }

    gl->prefix_len = len + 1;
    gl->last_res = gl->finddata.cFileName;
    gl->pattern = pattern;
    gl->done = 0;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_glob_read(hcnse_glob_t *gl, char *res)
{
    size_t len, temp;
    hcnse_err_t err;

    if (gl->done) {
        return HCNSE_DONE;
    }

    temp = hcnse_strlen(gl->last_res);

    len = gl->prefix_len + temp + 1;
    if (HCNSE_MAX_PATH_LEN < len) {
        return HCNSE_ERR_FILESYS_LONG_PATH;
    }

    hcnse_memmove(res, gl->pattern, gl->prefix_len);
    hcnse_memmove(res + gl->prefix_len, gl->last_res, temp + 1);

    if (FindNextFile(gl->dir, &gl->finddata) != 0) {

        err = hcnse_get_errno();
        if (err != HCNSE_OK && err != ERROR_NO_MORE_FILES) {
            gl->done = 1;
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "FindNextFile(%p, %p) failed for \"%s\"",
                gl->dir, &gl->finddata, gl->pattern);
            return err;
        }
        gl->last_res = gl->finddata.cFileName;
    }
    else {
        gl->done = 1;
    }

    return HCNSE_OK;
}

void
hcnse_glob_close(hcnse_glob_t *gl)
{
    FindClose(gl->dir);
}

const char *
hcnse_dir_current_name(hcnse_dir_t *dir)
{
    return dir->finddata.cFileName;
}

hcnse_err_t
hcnse_check_absolute_path(const char *path)
{
    /*
     * C:\path\to\foo.txt - absolute
     * C:path\to\foo.txt  - non-absolute
     * 
     * UNC-paths
     * \\server1\share\test\foo.txt
     * 
     */

    if (hcnse_isalpha(path[0]) && path[1] == ':' && path[2] == '\\') {
        return HCNSE_OK;
    }
    else if (path[0] == '\\' && path[1] == '\\') {
        return HCNSE_OK;
    }
    else {
        return HCNSE_ERR_FILESYS_ABS_PATH;
    }
}

#if 0
hcnse_err_t
hcnse_path_to_root_dir(char *res, const char *path)
{
    if (hcnse_isalpha(path[0]) && path[1] == ':' && path[2] == '\\') {
        hcnse_memmove(res, path, 3);
        res[3] = '\0';
        return res;
    }
    else if (path[0] == '\\' && path[1] == '\\') {
        hcnse_memmove(res, path, 2);
        res[2] = '\0';
        return HCNSE_OK;
    }
    else {
        return HCNSE_ERR_FILESYS_ABS_PATH;
    }
}
#endif

#endif

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

bool
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
