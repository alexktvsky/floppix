#include "hcnse_portable.h"
#include "hcnse_core.h"


#if (HCNSE_POSIX)
hcnse_err_t
hcnse_file_init(hcnse_file_t *file, const char *name, int mode, int create,
    int access)
{
    hcnse_fd_t fd;
    hcnse_err_t err;

    fd = hcnse_open_file(name, mode, create, access);
    if (fd == HCNSE_INVALID_FILE) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "open(\"%s\", %d|%d, %o) failed", name, mode, create, access);
        return err;
    }
    file->fd = fd;
    file->name = (char *) name;
    file->offset = (size_t) 0;
    return HCNSE_OK;   
}

ssize_t
hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size, off_t offset)
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
hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size, off_t offset)
{
    ssize_t n, written;
    
    written = 0;

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
    size_t size;
    off_t offset;

    offset = lseek(file->fd, 0, SEEK_CUR);
    if (offset == (off_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_CUR) failed", file->fd, 0);
        return -1;
    }
    size = (size_t) lseek(file->fd, 0, SEEK_END);
    if (size == (size_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_END) failed", file->fd, offset);
        return -1;
    }
    if (lseek(file->fd, offset, SEEK_SET) == (off_t) -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "lseek(%d, %zd, SEEK_SET) failed", file->fd, 0);
        return -1;
    }
    return size;
}

ssize_t
server_read_fd(hcnse_fd_t fd, void *buf, size_t n)
{
    return read(fd, buf, n);
}

ssize_t
hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t n)
{
    return write(fd, buf, n);
}


#elif (HCNSE_WIN32)
hcnse_err_t
hcnse_file_init(hcnse_file_t *file, const char *name, int mode, int create,
    int access)
{
    hcnse_fd_t fd;
    hcnse_err_t err;
    (void) access;

    fd = hcnse_open_file(name, mode, create, access);
    if (fd == HCNSE_INVALID_FILE) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "CreateFile(\"%s\", %d, %d, %p, %d, %d, %p) failed",
            name, mode, 0, NULL, create, 0, NULL);
        return err;
    }
    file->fd = fd;
    file->name = (char *) name;
    file->offset = (size_t) 0;
    return HCNSE_OK;
}

ssize_t
hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size, off_t offset)
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
            "ReadFile(%d, %p, %zu, %p, %p) failed",
            file->fd, buf, size, &n, &ovlp);
        return -1;
    }
    file->fd += n;
    return (ssize_t) n;
}

ssize_t
hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size, off_t offset)
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
            "WriteFile(%d, %p, %zu, %p, %p) failed",
            file->fd, buf, size, &n, &ovlp);
        return -1;
    }

    if (((size_t) n) != size) {
        return -1;
    }
    file->offset += n;
    return n;
}

ssize_t
hcnse_file_size(hcnse_file_t *file)
{
    LARGE_INTEGER info;
    ssize_t size;

    if (GetFileSizeEx(file->fd, &info) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "GetFileSizeEx(%d, %p) failed",
            file->fd, &info);
        return -1;
    }
    size = info.QuadPart;
    return size;
}

ssize_t
server_read_fd(hcnse_fd_t fd, void *buf, size_t size)
{
    DWORD n;
    if (ReadFile(fd, buf, size, &n, NULL) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "ReadFile(%d, %p, %zu, %p, %p) failed",
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
            "WriteFile(%d, %p, %zu, %p, %p) failed",
            fd, buf, size, &n, NULL);
        return -1;
    }
    return (ssize_t) n;
}
#endif

ssize_t
hcnse_file_write1(hcnse_file_t *file, const char *buf, size_t size)
{
    return hcnse_file_write(file, buf, size, file->offset);
}


ssize_t
hcnse_write_stdout(const char *str)
{
    return hcnse_write_fd(HCNSE_STDOUT, (void *) str, hcnse_strlen(str));
}

ssize_t
hcnse_write_stderr(const char *str)
{
    return hcnse_write_fd(HCNSE_STDERR, (void *) str, hcnse_strlen(str));
}

void
hcnse_file_fini(hcnse_file_t *file)
{
    hcnse_close_file(file->fd);
    file->fd = HCNSE_INVALID_FILE;
    file->name = NULL;
    file->offset = (off_t) 0;
}
