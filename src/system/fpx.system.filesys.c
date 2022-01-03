#include "fpx.system.filesys.h"
#include "fpx.system.memory.h"
#include "fpx.util.string.h"
#include "fpx.core.log.h"

#if (FPX_POSIX)

static fpx_file_type_t
fpx_stat_get_file_type(fpx_file_stat_t *stat)
{
    fpx_file_type_t type;
    mode_t mode;

    type = FPX_FILE_TYPE_UNKFILE;
    mode = stat->st_mode;

    if (S_ISREG(mode)) {
        type = FPX_FILE_TYPE_FILE;
    }
    if (S_ISDIR(mode)) {
        type = FPX_FILE_TYPE_DIR;
    }
    if (S_ISCHR(mode)) {
        type = FPX_FILE_TYPE_CHR;
    }

#if defined(S_ISBLK)
    if (S_ISBLK(mode)) {
        type = FPX_FILE_TYPE_BLK;
    }
#endif

#if defined(S_ISFIFO)
    if (S_ISFIFO(mode)) {
        type = FPX_FILE_TYPE_PIPE;
    }
#endif

#if defined(S_ISLNK)
    if (S_ISLNK(mode)) {
        type = FPX_FILE_TYPE_LINK;
    }
#endif

#if defined(S_ISLNK)
    if (S_ISSOCK(mode)) {
        type = FPX_FILE_TYPE_SOCK;
    }
#endif

    return type;
}

static fpx_file_access_t
fpx_stat_get_access(fpx_file_stat_t *stat)
{
    fpx_file_access_t access;
    mode_t mode;

    access = 0;
    mode = stat->st_mode;

    if (mode & S_ISUID) {
        access |= FPX_FILE_ACCESS_USETID;
    }
    if (mode & S_IRUSR) {
        access |= FPX_FILE_ACCESS_UR;
    }
    if (mode & S_IWUSR) {
        access |= FPX_FILE_ACCESS_UW;
    }
    if (mode & S_IXUSR) {
        access |= FPX_FILE_ACCESS_UX;
    }

    if (mode & S_ISGID) {
        access |= FPX_FILE_ACCESS_GSETID;
    }
    if (mode & S_IRGRP) {
        access |= FPX_FILE_ACCESS_GR;
    }
    if (mode & S_IWGRP) {
        access |= FPX_FILE_ACCESS_GW;
    }
    if (mode & S_IXGRP) {
        access |= FPX_FILE_ACCESS_GX;
    }

#if defined(S_ISVTX)
    if (mode & S_ISVTX) {
        access |= FPX_FILE_ACCESS_STICKY;
    }
#endif

    if (mode & S_IROTH) {
        access |= FPX_FILE_ACCESS_OR;
    }
    if (mode & S_IWOTH) {
        access |= FPX_FILE_ACCESS_OW;
    }
    if (mode & S_IXOTH) {
        access |= FPX_FILE_ACCESS_OX;
    }

    return access;
}

fpx_err_t
fpx_file_open(fpx_file_t *file, const char *path, fpx_uint_t mode,
    fpx_uint_t create, fpx_uint_t access)
{
    fpx_fd_t fd;
    fpx_err_t err;

    fpx_memzero(file, sizeof(fpx_file_t));

    fd = open(path, mode | create, access);
    if (fd == FPX_INVALID_FILE) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "open(\"%s\", %zu|%zu, %o) failed",
            path, mode, create, access);
        return err;
    }

    file->fd = fd;
    file->name = (char *) path;
    file->offset = (fpx_size_t) 0;

    return FPX_OK;
}

fpx_ssize_t
fpx_file_read(fpx_file_t *file, uint8_t *buf, fpx_size_t size, fpx_off_t offset)
{
    fpx_ssize_t n;

    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "lseek(%d, %zd, SEEK_SET) failed", file->fd, offset);
        return -1;
    }
    n = read(file->fd, buf, size);
    if (n == -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "read(%d, %p, %zu) failed", file->fd, buf, size);
        return -1;
    }
    file->offset += n;
    return n;
}

fpx_ssize_t
fpx_file_write(fpx_file_t *file, const char *buf, fpx_size_t size,
    fpx_off_t offset)
{
    fpx_ssize_t n;
    fpx_ssize_t written = 0;

    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "lseek(%d, %zd, %s) failed", file->fd, offset, fpx_value(SEEK_SET));
        return -1;
    }

    for (;;) {
        n = write(file->fd, buf + written, size);
        if (n == -1) {
            fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
                "write(%d, %p, %zu) failed", file->fd, buf + written, size);
            return -1;
        }
        file->offset += n;
        written += n;
        if ((fpx_size_t) n == size) {
            return written;
        }
        size -= n;
    }
}

/* Work even we already read some bytes from file */
fpx_ssize_t
fpx_file_size(fpx_file_t *file)
{
    fpx_off_t off;
    fpx_size_t size;

    off = lseek(file->fd, 0, SEEK_CUR);
    if (off == (fpx_off_t) -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "lseek(%d, %zd, SEEK_CUR) failed", file->fd, 0);
        return -1;
    }
    size = (fpx_size_t) lseek(file->fd, 0, SEEK_END);
    if (size == (fpx_size_t) -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "lseek(%d, %zd, SEEK_END) failed", file->fd, 0);
        return -1;
    }
    if (lseek(file->fd, off, SEEK_SET) == (fpx_off_t) -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "lseek(%d, %zd, SEEK_SET) failed", file->fd, 0);
        return -1;
    }
    return size;
}

fpx_ssize_t
fpx_read_fd(fpx_fd_t fd, void *buf, fpx_size_t n)
{
    fpx_ssize_t rv;

    rv = read(fd, buf, n);
    if (rv == -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "read(%d, %p, %zu) failed", fd, buf, n);
        return -1;
    }
    return rv;
}

fpx_ssize_t
fpx_write_fd(fpx_fd_t fd, void *buf, fpx_size_t n)
{
    fpx_ssize_t rv;

    rv = write(fd, buf, n);
    if (rv == -1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "write(%d, %p, %zu) failed", fd, buf, n);
        return -1;
    }
    return rv;
}

fpx_err_t
fpx_file_info_by_path(fpx_file_info_t *info, const char *path)
{
    fpx_err_t err;

    if (stat(path, &(info->stat)) == -1) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "stat(\"%s\", %p) failed", path,
            info);
        return err;
    }
    info->type = fpx_stat_get_file_type(&(info->stat));
    info->access = fpx_stat_get_access(&(info->stat));

    return FPX_OK;
}

fpx_err_t
fpx_file_info(fpx_file_info_t *info, fpx_file_t *file)
{
    fpx_err_t err;

    if (fstat(file->fd, &(info->stat)) == -1) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "fstat(%d, %p) failed", file->fd,
            info);
        return err;
    }
    info->type = fpx_stat_get_file_type(&(info->stat));
    info->access = fpx_stat_get_access(&(info->stat));

    return FPX_OK;
}

void
fpx_file_close(fpx_file_t *file)
{
    if (!file) {
        return;
    }

    close(file->fd);
    file->fd = FPX_INVALID_FILE;
    file->name = NULL;
    file->offset = (fpx_off_t) 0;
}

fpx_err_t
fpx_dir_open(fpx_dir_t *dir, const char *path)
{
    fpx_err_t err;

    fpx_memzero(dir, sizeof(fpx_dir_t));

    dir->dir = opendir(path);
    if (dir->dir == NULL) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "opendir(%p, \"%s\") failed", dir,
            path);
        return err;
    }

    dir->name = path;
    /*
     * Set by memset()
     * dir-type = FPX_FILE_TYPE_UNKFILE;
     */

    return FPX_OK;
}

#if (FPX_HAVE_D_TYPE)

fpx_err_t
fpx_dir_read(fpx_dir_t *dir)
{
    fpx_err_t err;

    dir->dirent = readdir(dir->dir);
    if (!dir->dirent) {
        err = fpx_get_errno();
        if (err != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, err, "readdir(%p) failed for \"%s\"",
                dir, dir->name);
            return err;
        }
        return FPX_DONE;
    }

    dir->type = dir->dirent->d_type;

    return FPX_OK;
}
#else

fpx_err_t
fpx_dir_read(fpx_dir_t *dir)
{
    fpx_file_stat_t fstat;
    char fullpath[FPX_MAX_PATH_LEN];
    fpx_err_t err;

    dir->dirent = readdir(dir->dir);
    if (!dir->dirent) {
        err = fpx_get_errno();
        if (err != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, err, "readdir(%p) failed for \"%s\"",
                dir, dir->name);
            return err;
        }
        return FPX_DONE;
    }

    fpx_file_full_path(fullpath, dir->name, dir->dirent->d_name);

    fpx_file_stat_by_path(&fstat, fullpath);

    dir->type = fpx_stat_get_file_type(&fstat);

    return FPX_OK;
}
#endif

void
fpx_dir_close(fpx_dir_t *dir)
{
    if (!dir) {
        return;
    }
    closedir(dir->dir);
}

const char *
fpx_dir_current_name(fpx_dir_t *dir)
{
    return dir->dirent->d_name;
}

fpx_err_t
fpx_glob_open(fpx_glob_t *gl, const char *pattern)
{
    fpx_int_t rv;
    fpx_err_t err;

    rv = glob(pattern, 0, NULL, &gl->glob);
    if (rv != 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "glob(%s, %p) failed", gl->pattern,
            &gl->glob);
        return err;
    }

    gl->pattern = pattern;
    gl->last = 0;

    return FPX_OK;
}

fpx_err_t
fpx_glob_read(fpx_glob_t *gl, char *res)
{
    fpx_size_t n, len;

#ifdef GLOB_NOMATCH
    n = (fpx_size_t) gl->glob.gl_pathc;
#else
    n = (fpx_size_t) gl->glob.gl_matchc;
#endif

    if (gl->last >= n) {
        return FPX_DONE;
    }

    len = strlen(gl->glob.gl_pathv[gl->last]);
    memmove(res, gl->glob.gl_pathv[gl->last], len + 1);
    gl->last += 1;

    return FPX_OK;
}

void
fpx_glob_close(fpx_glob_t *gl)
{
    globfree(&gl->glob);
}

fpx_err_t
fpx_check_absolute_path(const char *path)
{
    if (*path == '/') {
        return FPX_OK;
    }

    return FPX_ERR_FILESYS_ABS_PATH;
}

#elif (FPX_WIN32)

static uint32_t
fpx_utf8_decode(u_char **p, fpx_size_t n)
{
    fpx_size_t len;
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
fpx_path_to_wchar(uint16_t *outstr, fpx_size_t outlen, const uint8_t *instr)
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

        n = fpx_utf8_decode(&p, 4);

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

static fpx_file_type_t
fpx_stat_get_file_type(fpx_file_stat_t *stat)
{
    fpx_file_type_t type;

    type = FPX_FILE_TYPE_UNKFILE;

    if (stat->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        type = FPX_FILE_TYPE_DIR;
    }
    if (!(stat->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        type = FPX_FILE_TYPE_FILE;
    }

    return type;
}

fpx_err_t
fpx_file_open(fpx_file_t *file, const char *path, fpx_uint_t mode,
    fpx_uint_t create, fpx_uint_t access)
{
    uint16_t wpath[FPX_MAX_PATH_LEN];
    fpx_fd_t fd;
    fpx_err_t err;

    (void) access;

    fpx_memzero(file, sizeof(fpx_file_t));

    fpx_path_to_wchar(wpath, sizeof(wpath), (const uint8_t *) path);

    fd = CreateFileW(wpath, mode, 0, NULL, create, 0, NULL);
    if (fd == FPX_INVALID_FILE) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "CreateFileW(\"%s\", %zu, %d, %p, %zu, %d, %p) failed", path, mode,
            0, NULL, create, 0, NULL);
        return err;
    }

    file->fd = fd;
    file->name = (char *) path;
    file->offset = (fpx_size_t) 0;

    return FPX_OK;
}

fpx_ssize_t
fpx_file_read(fpx_file_t *file, uint8_t *buf, fpx_size_t size, fpx_off_t offset)
{
    DWORD n;
    OVERLAPPED ovlp;

    ovlp.Internal = 0;
    ovlp.InternalHigh = 0;
    ovlp.Offset = (DWORD) offset;
    /* ovlp.OffsetHigh = (DWORD) (offset >> 32); */
    ovlp.hEvent = NULL;

    if (ReadFile(file->fd, buf, size, &n, &ovlp) == 0) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "ReadFile(%p, %p, %zu, %p, %p) failed", file->fd, buf, size, &n,
            &ovlp);
        return -1;
    }
    file->offset += n;
    return (fpx_ssize_t) n;
}

fpx_ssize_t
fpx_file_write(fpx_file_t *file, const char *buf, fpx_size_t size,
    fpx_off_t offset)
{
    DWORD n;
    OVERLAPPED ovlp;

    ovlp.Internal = 0;
    ovlp.InternalHigh = 0;
    ovlp.Offset = (DWORD) offset;
    /* ovlp.OffsetHigh = (DWORD) (offset >> 32); */
    ovlp.hEvent = NULL;

    if (WriteFile(file->fd, buf, size, &n, &ovlp) == 0) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "WriteFile(%p, %p, %zu, %p, %p) failed", file->fd, buf, size, &n,
            &ovlp);
        return -1;
    }

    if (((fpx_size_t) n) != size) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED, "((fpx_size_t) n) != size");
        return -1;
    }
    file->offset += n;
    return n;
}

#if (FPX_PTR_SIZE == 8)

fpx_ssize_t
fpx_file_size(fpx_file_t *file)
{
    LARGE_INTEGER info;
    fpx_ssize_t size;

    if (GetFileSizeEx(file->fd, &info) != 1) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "GetFileSizeEx(%p, %p) failed", file->fd, &info);
        return -1;
    }
    size = info.QuadPart;

    return (fpx_ssize_t) size;
}
#else

fpx_ssize_t
fpx_file_size(fpx_file_t *file)
{
    fpx_ssize_t size;

    size = (fpx_ssize_t) GetFileSize(file->fd, NULL);
    if (size == FPX_INVALID_FILE_SIZE) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "GetFileSize(%p, %p) failed", file->fd, NULL);
        return -1;
    }
    return (fpx_ssize_t) size;
}
#endif

fpx_ssize_t
fpx_read_fd(fpx_fd_t fd, void *buf, fpx_size_t size)
{
    DWORD n;

    if (ReadFile(fd, buf, size, &n, NULL) == 0) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "ReadFile(%p, %p, %zu, %p, %p) failed", fd, buf, size, &n, NULL);
        return -1;
    }
    return (fpx_ssize_t) n;
}

fpx_ssize_t
fpx_write_fd(fpx_fd_t fd, void *buf, fpx_size_t size)
{
    DWORD n;

    if (WriteFile(fd, buf, size, &n, NULL) == 0) {
        fpx_log_error1(FPX_LOG_ERROR, fpx_get_errno(),
            "WriteFile(%p, %p, %zu, %p, %p) failed", fd, buf, size, &n, NULL);
        return -1;
    }
    return (fpx_ssize_t) n;
}

fpx_err_t
fpx_file_stat_by_path(fpx_file_stat_t *stat, const char *path)
{
    WIN32_FILE_ATTRIBUTE_DATA file_attr;
    uint16_t wpath[FPX_MAX_PATH_LEN];
    fpx_err_t err;

    fpx_path_to_wchar(wpath, sizeof(wpath), (const uint8_t *) path);

    if (GetFileAttributesExW(wpath, GetFileExInfoStandard, &file_attr) == 0) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "GetFileAttributesExW(\"%s\") failed", path);
        return err;
    }

    stat->dwFileAttributes = file_attr.dwFileAttributes;
    stat->ftCreationTime = file_attr.ftCreationTime;
    stat->ftLastAccessTime = file_attr.ftLastAccessTime;
    stat->ftLastWriteTime = file_attr.ftLastWriteTime;
    stat->nFileSizeHigh = file_attr.nFileSizeHigh;
    stat->nFileSizeLow = file_attr.nFileSizeLow;

    return FPX_OK;
}

fpx_err_t
fpx_file_info_by_path(fpx_file_info_t *info, const char *path)
{
    fpx_err_t

        err = fpx_file_stat_by_path(&(info->stat), path);
    if (err != FPX_OK) {
        return err;
    }

    info->type = fpx_stat_get_file_type(&(info->stat));
    info->access = 0;

    return FPX_OK;
}

fpx_err_t
fpx_file_info(fpx_file_info_t *info, fpx_file_t *file)
{
    fpx_err_t err;

    if (GetFileInformationByHandle(file->fd, &(info->stat)) == -1) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err,
            "GetFileInformationByHandle(%d) failed", file->fd);
        return err;
    }

    info->type = fpx_stat_get_file_type(&(info->stat));
    info->access = 0;

    return FPX_OK;
}

void
fpx_file_close(fpx_file_t *file)
{
    if (!file) {
        return;
    }

    CloseHandle(file->fd);
    file->fd = FPX_INVALID_FILE;
    file->name = NULL;
    file->offset = (fpx_off_t) 0;
}

fpx_err_t
fpx_dir_open(fpx_dir_t *dir, const char *path)
{
    char str[FPX_MAX_PATH_LEN + 3];
    fpx_size_t len;
    fpx_err_t err;

    fpx_memzero(dir, sizeof(fpx_dir_t));

    len = fpx_strlen(path);

    if (len >= FPX_MAX_PATH_LEN + 3) {
        return E2BIG;
    }

    fpx_memmove(str, path, len);
    str[len + 0] = '\\';
    str[len + 1] = '*';
    str[len + 2] = '\0';

    dir->dir = FindFirstFile((const char *) str, &(dir->finddata));
    if (dir->dir == INVALID_HANDLE_VALUE) {
        err = fpx_get_errno();
        if (err != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, err,
                "FindFirstFile(\"%s\", %p) failed", str, &(dir->finddata));
            return err;
        }
        return FPX_DONE;
    }

    dir->name = path;
    /*
     * Set by memset()
     * dir-type = FPX_FILE_TYPE_UNKFILE;
     */

    return FPX_OK;
}

fpx_err_t
fpx_dir_read(fpx_dir_t *dir)
{
    fpx_file_stat_t fstat;
    char fullpath[FPX_MAX_PATH_LEN];
    fpx_err_t err;

    if (FindNextFile(dir->dir, &dir->finddata) == 0) {
        err = fpx_get_errno();
        if (err != FPX_OK && err != ERROR_NO_MORE_FILES) {
            fpx_log_error1(FPX_LOG_ERROR, err,
                "FindNextFile(%p) failed for \"%s\"", dir->dir, &dir->finddata,
                dir->name);
            return err;
        }
        return FPX_DONE;
    }

    fpx_file_full_path(fullpath, dir->name, dir->finddata.cFileName);

    fpx_file_stat_by_path(&fstat, fullpath);

    dir->type = fpx_stat_get_file_type(&fstat);

    return FPX_OK;
}

void
fpx_dir_close(fpx_dir_t *dir)
{
    if (!dir) {
        return;
    }
    FindClose(dir->dir);
}

fpx_err_t
fpx_glob_open(fpx_glob_t *gl, const char *pattern)
{
    fpx_int_t i;
    fpx_size_t len;
    fpx_err_t err;

    gl->dir = FindFirstFile((const char *) pattern, &gl->finddata);
    if (gl->dir == INVALID_HANDLE_VALUE) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "FindFirstFile(%s, %p) failed",
            pattern, &gl->finddata);
        return FPX_FAILED;
    }

    len = 0;

    for (i = fpx_strlen(pattern); i >= 0; --i) {
        if (pattern[i] == '\\') {
            len = (fpx_size_t) i;
            break;
        }
    }

    if (len == 0) {
        return FPX_ERR_FILESYS_ABS_PATH;
    }

    gl->prefix_len = len + 1;
    gl->last_res = gl->finddata.cFileName;
    gl->pattern = pattern;
    gl->done = 0;

    return FPX_OK;
}

fpx_err_t
fpx_glob_read(fpx_glob_t *gl, char *res)
{
    fpx_size_t len, temp;
    fpx_err_t err;

    if (gl->done) {
        return FPX_DONE;
    }

    temp = fpx_strlen(gl->last_res);

    len = gl->prefix_len + temp + 1;
    if (FPX_MAX_PATH_LEN < len) {
        return FPX_ERR_FILESYS_LONG_PATH;
    }

    fpx_memmove(res, gl->pattern, gl->prefix_len);
    fpx_memmove(res + gl->prefix_len, gl->last_res, temp + 1);

    if (FindNextFile(gl->dir, &gl->finddata) != 0) {

        err = fpx_get_errno();
        if (err != FPX_OK && err != ERROR_NO_MORE_FILES) {
            gl->done = 1;
            fpx_log_error1(FPX_LOG_ERROR, err,
                "FindNextFile(%p, %p) failed for \"%s\"", gl->dir,
                &gl->finddata, gl->pattern);
            return err;
        }
        gl->last_res = gl->finddata.cFileName;
    }
    else {
        gl->done = 1;
    }

    return FPX_OK;
}

void
fpx_glob_close(fpx_glob_t *gl)
{
    FindClose(gl->dir);
}

const char *
fpx_dir_current_name(fpx_dir_t *dir)
{
    return dir->finddata.cFileName;
}

fpx_err_t
fpx_check_absolute_path(const char *path)
{
    /*
     * C:\path\to\foo.txt - absolute
     * C:path\to\foo.txt  - non-absolute
     *
     * UNC-paths
     * \\server1\share\test\foo.txt
     *
     */

    if (fpx_isalpha(path[0]) && path[1] == ':' && path[2] == '\\') {
        return FPX_OK;
    }
    else if (path[0] == '\\' && path[1] == '\\') {
        return FPX_OK;
    }
    else {
        return FPX_ERR_FILESYS_ABS_PATH;
    }
}

#if 0
fpx_err_t
fpx_path_to_root_dir(char *res, const char *path)
{
    if (fpx_isalpha(path[0]) && path[1] == ':' && path[2] == '\\') {
        fpx_memmove(res, path, 3);
        res[3] = '\0';
        return res;
    }
    else if (path[0] == '\\' && path[1] == '\\') {
        fpx_memmove(res, path, 2);
        res[2] = '\0';
        return FPX_OK;
    }
    else {
        return FPX_ERR_FILESYS_ABS_PATH;
    }
}
#endif

#endif

fpx_ssize_t
fpx_write_stdout(const char *str)
{
    return fpx_write_fd(FPX_STDOUT, (void *) str, strlen(str));
}

fpx_ssize_t
fpx_write_stderr(const char *str)
{
    return fpx_write_fd(FPX_STDERR, (void *) str, strlen(str));
}

fpx_file_type_t
fpx_dir_current_file_type(fpx_dir_t *dir)
{
    return dir->type;
}

fpx_err_t
fpx_dir_current_is_file(fpx_dir_t *dir)
{
    return (dir->type == FPX_FILE_TYPE_FILE);
}

fpx_size_t
fpx_file_full_path(char *buf, const char *path, const char *file)
{
    fpx_size_t path_len, file_len, copied;

    path_len = fpx_strlen(path);
    file_len = fpx_strlen(file);

    if (fpx_is_path_separator(path[path_len - 1])) {
        path_len -= 1;
    }

    copied = 0;

    fpx_memmove(buf + copied, path, path_len);
    copied += path_len;

    fpx_memmove(buf + copied, FPX_PATH_SEPARATOR_STR, 1);
    copied += +1;

    fpx_memmove(buf + copied, file, file_len);
    copied += file_len;

    buf[copied] = '\0';

    return copied;
}

fpx_bool_t
fpx_is_path_has_wildcard(const char *path)
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
