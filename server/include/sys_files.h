#ifndef INCLUDED_SYS_FILES_H
#define INCLUDED_SYS_FILES_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "errors.h"
#include "syshead.h"

#if (SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SYS_STDIN                    STDIN_FILENO
#define SYS_STDOUT                   STDOUT_FILENO
#define SYS_STDERR                   STDERR_FILENO

/* mode */
#define SYS_FILE_RDONLY              O_RDONLY
#define SYS_FILE_WRONLY              O_WRONLY
#define SYS_FILE_RDWR                O_RDWR

/* create */
#define SYS_FILE_OPEN                0 // Don't open if already existing
#define SYS_FILE_CREATE_OR_OPEN      O_CREAT
#define SYS_FILE_TRUNCATE            (O_CREAT|O_TRUNC)
#define SYS_FILE_APPEND              (O_WRONLY|O_APPEND)
#define SYS_FILE_NONBLOCK            O_NONBLOCK

/* access */
#define SYS_FILE_DEFAULT_ACCESS      0644
#define SYS_FILE_OWNER_ACCESS        0600

#define SYS_INVALID_FILE            -1

#define open_file(name, mode, create, access) \
    open((const char *) name, mode|create, access)

#define close_file(fd)               close(fd)
#define delete_file(name)            unlink((const char *) name)

typedef int fd_t;


#elif (SYSTEM_WINDOWS)
#include <windows.h> /* fileapi.h */

#define SYS_STDIN                    GetStdHandle(STD_INPUT_HANDLE)
#define SYS_STDOUT                   GetStdHandle(STD_OUTPUT_HANDLE)
#define SYS_STDERR                   GetStdHandle(STD_ERROR_HANDLE)

/* mode */
#define SYS_FILE_RDONLY              GENERIC_READ
#define SYS_FILE_WRONLY              GENERIC_WRITE
#define SYS_FILE_RDWR                GENERIC_READ|GENERIC_WRITE

/* create */
#define SYS_FILE_CREATE_OR_OPEN      OPEN_ALWAYS
#define SYS_FILE_OPEN                OPEN_EXISTING
#define SYS_FILE_TRUNCATE            CREATE_ALWAYS
#define SYS_FILE_APPEND              FILE_APPEND_DATA|SYNCHRONIZE
#define SYS_FILE_NONBLOCK            0

/* access */
#define SYS_FILE_DEFAULT_ACCESS      0
#define SYS_FILE_OWNER_ACCESS        0

#define SYS_INVALID_FILE             INVALID_HANDLE_VALUE

#define open_file(name, mode, create, access) \
    CreateFile(TEXT(name), mode, 0, NULL, create, 0, NULL)

#define close_file(fd)               CloseHandle(fd)
#define delete_file(name)            DeleteFile((const char *) name)

typedef HANDLE fd_t;

#endif

typedef struct file_s {
    fd_t fd;
    char *name;
    off_t offset;
} sys_file_t;


err_t file_init(sys_file_t *file, const char *fname, int mode, int create, int access);
ssize_t file_read(sys_file_t *file, uint8_t *buf, size_t size, off_t offset);
ssize_t file_write(sys_file_t *file, const char *buf, size_t size, off_t offset);
ssize_t file_size(sys_file_t *file);
void file_fini(sys_file_t *file);

ssize_t write_stdout(const char *str);
ssize_t write_stderr(const char *str);


#endif /* INCLUDED_SYS_FILES_H */
