#ifndef FPX_SYSTEM_FILESYS_H
#define FPX_SYSTEM_FILESYS_H

#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"
#include "fpx.system.type.h"

#if (FPX_POSIX)

/* Mode */
#define FPX_FILE_RDONLY                   O_RDONLY
#define FPX_FILE_WRONLY                   O_WRONLY
#define FPX_FILE_RDWR                     O_RDWR

/* Create */
#define FPX_FILE_OPEN                     0 /* Don't open if already existing */
#define FPX_FILE_CREATE_OR_OPEN           O_CREAT
#define FPX_FILE_TRUNCATE                 (O_CREAT|O_TRUNC)
#define FPX_FILE_APPEND                   (O_WRONLY|O_APPEND)
#define FPX_FILE_CREATE_OR_APPEND         (O_CREAT|O_APPEND|O_WRONLY)
#define FPX_FILE_NONBLOCK                 O_NONBLOCK

/* Access */
#define FPX_FILE_DEFAULT_ACCESS           0644
#define FPX_FILE_OWNER_ACCESS             0600

#define FPX_INVALID_FILE                 -1
#define FPX_INVALID_FILE_SIZE            -1
#define FPX_FILE_ERROR                   -1

#define FPX_STDIN                         STDIN_FILENO
#define FPX_STDOUT                        STDOUT_FILENO
#define FPX_STDERR                        STDERR_FILENO

#define FPX_PATH_SEPARATOR_STR            "/"
#define FPX_PATH_SEPARATOR                '/'

#if defined(PATH_MAX)
#define FPX_MAX_PATH_LEN                  PATH_MAX
#else
/* #define FPX_MAX_PATH_LEN                  pathconf(path, _PC_PATH_MAX) */
#define FPX_MAX_PATH_LEN                  4096
#endif

#define FPX_FILE_ACCESS_USETID            0x8000
#define FPX_FILE_ACCESS_UR                0x0400
#define FPX_FILE_ACCESS_UW                0x0200
#define FPX_FILE_ACCESS_UX                0x0100

#define FPX_FILE_ACCESS_GSETID            0x4000
#define FPX_FILE_ACCESS_GR                0x0040
#define FPX_FILE_ACCESS_GW                0x0020
#define FPX_FILE_ACCESS_GX                0x0010

#define FPX_FILE_ACCESS_STICKY            0x2000
#define FPX_FILE_ACCESS_OR                0x0004
#define FPX_FILE_ACCESS_OW                0x0002
#define FPX_FILE_ACCESS_OX                0x0001

#if (FPX_HAVE_D_TYPE)
#define FPX_FILE_TYPE_UNKFILE  DT_UNKNOWN /* This is an unknown type */
#define FPX_FILE_TYPE_FILE     DT_REG     /* This is a regular file */
#define FPX_FILE_TYPE_DIR      DT_DIR     /* This is a directory */
#define FPX_FILE_TYPE_CHR      DT_CHR     /* This is a character device */
#define FPX_FILE_TYPE_BLK      DT_BLK     /* This is a block device */
#define FPX_FILE_TYPE_PIPE     DT_FIFO    /* This is a named pipe (FIFO) */
#define FPX_FILE_TYPE_LINK     DT_LNK     /* This is a symbolic link */
#define FPX_FILE_TYPE_SOCK     DT_SOCK    /* This is a UNIX domain socket */
#else
#define FPX_FILE_TYPE_UNKFILE  0          /* This is an unknown type */
#define FPX_FILE_TYPE_FILE     1          /* This is a regular file */
#define FPX_FILE_TYPE_DIR      2          /* This is a directory */
#define FPX_FILE_TYPE_CHR      3          /* This is a character device */
#define FPX_FILE_TYPE_BLK      4          /* This is a block device */
#define FPX_FILE_TYPE_PIPE     5          /* This is a named pipe (FIFO) */
#define FPX_FILE_TYPE_LINK     6          /* This is a symbolic link */
#define FPX_FILE_TYPE_SOCK     7          /* This is a UNIX domain socket */
#endif

typedef uintptr_t                      fpx_file_access_t;
typedef uintptr_t                      fpx_file_type_t;
typedef struct fpx_file_s            fpx_file_t;
typedef struct fpx_dir_s             fpx_dir_t;
typedef struct fpx_glob_s            fpx_glob_t;
typedef struct fpx_file_info_s       fpx_file_info_t;

struct fpx_dir_s {
    DIR *dir;
    struct dirent *dirent;
    const char *name;
    uint8_t type;
};

struct fpx_glob_s {
    glob_t glob;
    const char *pattern;
    size_t last;
};


#define fpx_file_change_access(name, access) \
    chmod((const char *) name, access)

#define fpx_file_rename(from, to) \
    rename((const char *) from, (const char *) to)

#define fpx_file_delete(name)             unlink((const char *) name)
#define fpx_dir_create(name, access)      mkdir((const char *) name, access)
#define fpx_dir_delete(name)              rmdir((const char *) name)

#define fpx_file_stat_by_path(st, path)   stat((const char *) path, st)
#define fpx_file_stat_by_fd(st, fd)       fstat(fd, st)
#define fpx_file_stat_by_link(st, path)   lstat((const char *) path, st)

#define fpx_fsync(fd)                     fsync(fd)

#define fpx_is_path_separator(c)          ((c) == '/')


#elif (FPX_WIN32)

/* Mode */
#define FPX_FILE_RDONLY                   GENERIC_READ
#define FPX_FILE_WRONLY                   GENERIC_WRITE
#define FPX_FILE_RDWR                     GENERIC_READ|GENERIC_WRITE

/* Create */
#define FPX_FILE_CREATE_OR_OPEN           OPEN_ALWAYS
#define FPX_FILE_OPEN                     OPEN_EXISTING
#define FPX_FILE_TRUNCATE                 CREATE_ALWAYS
#define FPX_FILE_APPEND                   FILE_APPEND_DATA|SYNCHRONIZE
#define FPX_FILE_CREATE_OR_APPEND         OPEN_ALWAYS /* fixme */
#define FPX_FILE_NONBLOCK                 0

/* Access */
#define FPX_FILE_DEFAULT_ACCESS           0
#define FPX_FILE_OWNER_ACCESS             0

#define FPX_INVALID_FILE                  INVALID_HANDLE_VALUE
#define FPX_INVALID_FILE_SIZE             INVALID_FILE_SIZE
#define FPX_FILE_ERROR                    0

#define FPX_STDIN                         GetStdHandle(STD_INPUT_HANDLE)
#define FPX_STDOUT                        GetStdHandle(STD_OUTPUT_HANDLE)
#define FPX_STDERR                        GetStdHandle(STD_ERROR_HANDLE)

#define FPX_PATH_SEPARATOR_STR            "\\"
#define FPX_PATH_SEPARATOR                '\\'

#define FPX_MAX_PATH_LEN                  MAX_PATH

#define FPX_FILE_TYPE_UNKFILE  0          /* This is an unknown type */
#define FPX_FILE_TYPE_FILE     1          /* This is a regular file */
#define FPX_FILE_TYPE_DIR      2          /* This is a directory */
#define FPX_FILE_TYPE_CHR      3          /* This is a character device */
#define FPX_FILE_TYPE_BLK      4          /* This is a block device */
#define FPX_FILE_TYPE_PIPE     5          /* This is a named pipe (FIFO) */
#define FPX_FILE_TYPE_LINK     6          /* This is a symbolic link */
#define FPX_FILE_TYPE_SOCK     7          /* This is a UNIX domain socket */

struct fpx_dir_s {
    HANDLE dir;
    WIN32_FIND_DATA finddata;
    const char *name;
    uint8_t type;
};

struct fpx_glob_s {
    HANDLE dir;
    WIN32_FIND_DATA finddata;
    const char *pattern;
    bool done;
    const char *last_res;
    size_t prefix_len;
};

#define fpx_file_rename(from, to) \
    MoveFile((const char *) from, (const char *) to)

#define fpx_file_delete(name)             DeleteFile((const char *) name)

#define fpx_file_stat_by_fd(fd, fi)       GetFileInformationByHandle(fd, fi)
#define fpx_file_stat_by_link(name, fi)   fpx_file_stat_by_path(fi, name)

#define fpx_fsync(fd)                     FlushFileBuffers(fd)

#define fpx_is_path_separator(c)          ((c) == '\\')

fpx_err_t fpx_file_stat_by_path(fpx_file_stat_t *stat, const char *path);

#endif

#define fpx_dir_current_namelen(dir) \
    fpx_strlen(fpx_dir_current_name(dir))

struct fpx_file_s {
    fpx_fd_t fd;
    char *name;
    fpx_off_t offset;
};

struct fpx_file_info_s {
    fpx_file_type_t type;
    fpx_file_access_t access;
    fpx_file_stat_t stat;
};


fpx_err_t fpx_file_open(fpx_file_t *file, const char *path, fpx_uint_t mode,
    fpx_uint_t create, fpx_uint_t access);
ssize_t fpx_file_read(fpx_file_t *file, uint8_t *buf, size_t size,
    fpx_off_t offset);
ssize_t fpx_file_write(fpx_file_t *file, const char *buf, size_t size,
    fpx_off_t offset);
ssize_t fpx_file_size(fpx_file_t *file);
void fpx_file_close(fpx_file_t *file);

ssize_t fpx_read_fd(fpx_fd_t fd, void *buf, size_t n);
ssize_t fpx_write_fd(fpx_fd_t fd, void *buf, size_t n);

ssize_t fpx_write_stdout(const char *str);
ssize_t fpx_write_stderr(const char *str);

fpx_err_t fpx_file_info_by_path(fpx_file_info_t *info, const char *path);
fpx_err_t fpx_file_info(fpx_file_info_t *info, fpx_file_t *file);

fpx_err_t fpx_dir_open(fpx_dir_t *dir, const char *path);
fpx_err_t fpx_dir_read(fpx_dir_t *dir);
const char *fpx_dir_name(fpx_dir_t *dir);
const char *fpx_dir_current_name(fpx_dir_t *dir);
fpx_file_type_t fpx_dir_current_file_type(fpx_dir_t *dir);
fpx_err_t fpx_dir_current_is_file(fpx_dir_t *dir);
void fpx_dir_close(fpx_dir_t *dir);

int fpx_glob_open(fpx_glob_t *gl, const char *pattern);
int fpx_glob_read(fpx_glob_t *gl, char *res);
void fpx_glob_close(fpx_glob_t *gl);

size_t fpx_file_full_path(char *buf, const char *path, const char *file);
bool fpx_is_path_has_wildcard(const char *path);
fpx_err_t fpx_check_absolute_path(const char *path);

#endif /* FPX_SYSTEM_FILESYS_H */
