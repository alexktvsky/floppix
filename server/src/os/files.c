#include <stdlib.h>
#include <string.h>

#include "os/files.h"
#include "server/errors.h"


#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)
hcnse_err_t
hcnse_file_init(hcnse_file_t *file, const char *name, int mode, int create,
    int access)
{
    hcnse_fd_t fd = hcnse_open_file(name, mode, create, access);
    if (fd == HCNSE_INVALID_FILE) {
        return hcnse_get_errno();
    }
    file->fd = fd;
    file->name = (char *) name;
    file->offset = (size_t) 0;
    return HCNSE_OK;   
}

ssize_t
hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size, off_t offset)
{
    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        return -1;
    }
    ssize_t n = read(file->fd, buf, size);
    if (n == -1) {
        return -1;
    }
    file->offset += n;
    return n;
}

ssize_t
hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size, off_t offset)
{
    ssize_t n;
    ssize_t written = 0;

    if (lseek(file->fd, offset, SEEK_SET) == -1) {
        return -1;
    }

    for ( ; ; ) {
        n = write(file->fd, buf + written, size);
        if (n == -1) {
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
    off_t off = lseek(file->fd, 0, SEEK_CUR);
    if (off == (off_t) -1) {
        return -1;
    }
    size_t size = (size_t) lseek(file->fd, 0, SEEK_END);
    if (size == (size_t) -1) {
        return -1;
    }
    if (lseek(file->fd, off, SEEK_SET) == (off_t) -1) {
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


#elif (HCNSE_WINDOWS)
hcnse_err_t
hcnse_file_init(hcnse_file_t *file, const char *name, int mode, int create,
    int access)
{
    (void) access;
    hcnse_fd_t fd = hcnse_open_file(name, mode, create, access);
    if (fd == HCNSE_INVALID_FILE) {
        return hcnse_get_errno();
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
    // ovlp.OffsetHigh = (DWORD) (offset >> 32);
    ovlp.hEvent = NULL;

    if (!ReadFile(file->fd, buf, size, &n, &ovlp)) {
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
    // ovlp.OffsetHigh = (DWORD) (offset >> 32);
    ovlp.hEvent = NULL;

    if (!WriteFile(file->fd, buf, size, &n, &ovlp)) {
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
    if (GetFileSizeEx(file->fd, &info) != TRUE) {
        return -1;
    }
    ssize_t size = info.QuadPart;
    return size;
}

ssize_t
server_read_fd(hcnse_fd_t fd, void *buf, size_t size)
{
    DWORD n;
    if (ReadFile(fd, buf, size, &n, NULL) != 0) {
        return (ssize_t) n;
    }
    return -1;
}

ssize_t
hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t size)
{
    DWORD n;
    if (WriteFile(fd, buf, size, &n, NULL) != 0) {
        return (ssize_t) n;
    }
    return -1;
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
    return hcnse_write_fd(HCNSE_STDOUT, (void *) str, strlen(str));
}

ssize_t
hcnse_write_stderr(const char *str)
{
    return hcnse_write_fd(HCNSE_STDERR, (void *) str, strlen(str));
}

void
hcnse_file_fini(hcnse_file_t *file)
{
    hcnse_close_file(file->fd);
    file->fd = HCNSE_INVALID_FILE;
    file->name = NULL;
    file->offset = (off_t) 0;
}
