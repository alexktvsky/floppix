#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "syshead.h"
#include "errors.h"
#include "files.h"


#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
err_t file_init(file_t *file, const char *name, int mode, int create, int access)
{

    fd_t fd = open_file(name, mode, create, access);
    if (fd == SYS_INVALID_FILE) {
        return ERR_FAILED;
    }
    file->fd = fd;
    file->name = (char *) name;
    file->offset = (size_t) 0;
    return OK;   
}

ssize_t file_read(file_t *file, uint8_t *buf, size_t size, off_t offset)
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

ssize_t file_write(file_t *file, const char *buf, size_t size, off_t offset)
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
ssize_t file_size(file_t *file)
{
    off_t off = lseek(file->fd, 0, SEEK_CUR);
    if (off == (off_t) -1) {
        return -1;
    }
    size_t size = (size_t) lseek(file->fd, 0, SEEK_END);
    if (size == (off_t) -1) {
        return -1;
    }
    if (lseek(file->fd, off, SEEK_SET) == (off_t) -1) {
        return -1;
    }
    return size;
}

ssize_t read_fd(fd_t fd, void *buf, size_t n)
{
    return read(fd, buf, n);
}

ssize_t write_fd(fd_t fd, void *buf, size_t n)
{
    return write(fd, buf, n);
}


#elif (SYSTEM_WINDOWS)
err_t file_init(file_t *file, const char *name, int mode, int create, int access)
{
    fd_t fd = open_file(name, mode, create, access);
    if (fd == SYS_INVALID_FILE) {
        return ERR_FAILED;
    }
    file->fd = fd;
    file->name = (char *) name;
    file->offset = (size_t) 0;
    return OK;
}

ssize_t file_read(file_t *file, uint8_t *buf, size_t size, off_t offset)
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

ssize_t file_write(file_t *file, const char *buf, size_t size, off_t offset)
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

ssize_t file_size(file_t *file)
{
    LARGE_INTEGER info;
    if (GetFileSizeEx(file->fd, &info) != TRUE) {
        return -1;
    }
    ssize_t size = info.QuadPart;
    return size;
}

ssize_t read_fd(fd_t fd, void *buf, size_t size)
{
    DWORD n;
    if (ReadFile(fd, buf, size, &n, NULL) != 0) {
        return (ssize_t) n;
    }
    return -1;
}

ssize_t write_fd(fd_t fd, void *buf, size_t size)
{
    DWORD n;
    if (WriteFile(fd, buf, size, &n, NULL) != 0) {
        return (ssize_t) n;
    }
    return -1;
}

#endif

ssize_t write_stdout(const char *str) {
    return write_fd(SYS_STDOUT, (void *) str, strlen(str));
}

ssize_t write_stderr(const char *str) {
    return write_fd(SYS_STDERR, (void *) str, strlen(str));
}

void file_fini(file_t *file)
{
    if (!file) {
        return;
    }
    close_file(file->fd);
    file->fd = SYS_INVALID_FILE;
    file->name = NULL;
    file->offset = (off_t) 0;
    return;
}
