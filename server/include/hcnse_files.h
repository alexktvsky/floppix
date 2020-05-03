#ifndef INCLUDED_HCNSE_FILES_H
#define INCLUDED_HCNSE_FILES_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_LINUX || HCNSE_FREEBSD || HCNSE_SOLARIS)

#define HCNSE_STDIN                    STDIN_FILENO
#define HCNSE_STDOUT                   STDOUT_FILENO
#define HCNSE_STDERR                   STDERR_FILENO

/* mode */
#define HCNSE_FILE_RDONLY              O_RDONLY
#define HCNSE_FILE_WRONLY              O_WRONLY
#define HCNSE_FILE_RDWR                O_RDWR

/* create */
#define HCNSE_FILE_OPEN                0 // Don't open if already existing
#define HCNSE_FILE_CREATE_OR_OPEN      O_CREAT
#define HCNSE_FILE_TRUNCATE            O_TRUNC
#define HCNSE_FILE_APPEND              O_APPEND
#define HCNSE_FILE_NONBLOCK            O_NONBLOCK

/* access */
#define HCNSE_FILE_DEFAULT_ACCESS      0644
#define HCNSE_FILE_OWNER_ACCESS        0600

#define HCNSE_INVALID_FILE             -1

#define hcnse_open_file(name, mode, create, access) \
    open((const char *) name, mode|create, access)

#define hcnse_close_file(fd)           close(fd)
#define hcnse_delete_file(name)        unlink((const char *) name)

typedef int hcnse_fd_t;


#elif (HCNSE_WINDOWS)

#define HCNSE_STDIN                    GetStdHandle(STD_INPUT_HANDLE)
#define HCNSE_STDOUT                   GetStdHandle(STD_OUTPUT_HANDLE)
#define HCNSE_STDERR                   GetStdHandle(STD_ERROR_HANDLE)

/* mode */
#define HCNSE_FILE_RDONLY              GENERIC_READ
#define HCNSE_FILE_WRONLY              GENERIC_WRITE
#define HCNSE_FILE_RDWR                GENERIC_READ|GENERIC_WRITE

/* create */
#define HCNSE_FILE_CREATE_OR_OPEN      OPEN_ALWAYS
#define HCNSE_FILE_OPEN                OPEN_EXISTING
#define HCNSE_FILE_TRUNCATE            CREATE_ALWAYS
#define HCNSE_FILE_APPEND              FILE_APPEND_DATA|SYNCHRONIZE
#define HCNSE_FILE_NONBLOCK            0

/* access */
#define HCNSE_FILE_DEFAULT_ACCESS      0
#define HCNSE_FILE_OWNER_ACCESS        0

#define HCNSE_INVALID_FILE             INVALID_HANDLE_VALUE

#define hcnse_open_file(name, mode, create, access) \
    CreateFile(TEXT(name), mode, 0, NULL, create, 0, NULL)

#define hcnse_close_file(fd)           CloseHandle(fd)
#define hcnse_delete_file(name)        DeleteFile((const char *) name)

typedef HANDLE hcnse_fd_t;

#endif

typedef struct hcnse_file_s {
    hcnse_fd_t fd;
    char *name;
    off_t offset;
} hcnse_file_t;


hcnse_err_t hcnse_file_init(hcnse_file_t *file, const char *fname, int mode,
    int create, int access);

ssize_t hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size,
    off_t offset);

ssize_t hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size,
    off_t offset);

ssize_t hcnse_file_write1(hcnse_file_t *file, const char *buf, size_t size);

ssize_t hcnse_file_size(hcnse_file_t *file);
void hcnse_file_fini(hcnse_file_t *file);

ssize_t hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t size);
ssize_t hcnse_write_stdout(const char *str);
ssize_t hcnse_write_stderr(const char *str);


#endif /* INCLUDED_HCNSE_FILES_H */
