#ifndef INCLUDED_HCNSE_FILESYS_H
#define INCLUDED_HCNSE_FILESYS_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#if (HCNSE_POSIX)

/* Mode */
#define HCNSE_FILE_RDONLY                   O_RDONLY
#define HCNSE_FILE_WRONLY                   O_WRONLY
#define HCNSE_FILE_RDWR                     O_RDWR

/* Create */
#define HCNSE_FILE_OPEN                     0 /* Don't open if already existing */
#define HCNSE_FILE_CREATE_OR_OPEN           O_CREAT
#define HCNSE_FILE_TRUNCATE                 (O_CREAT|O_TRUNC)
#define HCNSE_FILE_APPEND                   (O_WRONLY|O_APPEND)
#define HCNSE_FILE_CREATE_OR_APPEND         (O_CREAT|O_APPEND|O_WRONLY)
#define HCNSE_FILE_NONBLOCK                 O_NONBLOCK

/* Access */
#define HCNSE_FILE_DEFAULT_ACCESS           0644
#define HCNSE_FILE_OWNER_ACCESS             0600

#define HCNSE_INVALID_FILE                 -1
#define HCNSE_FILE_ERROR                   -1

#define HCNSE_STDIN                         STDIN_FILENO
#define HCNSE_STDOUT                        STDOUT_FILENO
#define HCNSE_STDERR                        STDERR_FILENO

#define HCNSE_PATH_SEPARATOR_STR            "/"
#define HCNSE_PATH_SEPARATOR                '/'

#if defined(PATH_MAX)
#define HCNSE_MAX_PATH_LEN                  PATH_MAX
#else
/* #define HCNSE_MAX_PATH_LEN                  pathconf(path, _PC_PATH_MAX) */
#define HCNSE_MAX_PATH_LEN                  4096
#endif

#define HCNSE_FILE_ACCESS_USETID            0x8000
#define HCNSE_FILE_ACCESS_UR                0x0400
#define HCNSE_FILE_ACCESS_UW                0x0200
#define HCNSE_FILE_ACCESS_UX                0x0100

#define HCNSE_FILE_ACCESS_GSETID            0x4000
#define HCNSE_FILE_ACCESS_GR                0x0040
#define HCNSE_FILE_ACCESS_GW                0x0020
#define HCNSE_FILE_ACCESS_GX                0x0010

#define HCNSE_FILE_ACCESS_STICKY            0x2000
#define HCNSE_FILE_ACCESS_OR                0x0004
#define HCNSE_FILE_ACCESS_OW                0x0002
#define HCNSE_FILE_ACCESS_OX                0x0001

#if (HCNSE_HAVE_D_TYPE)
#define HCNSE_FILE_TYPE_UNKFILE  DT_UNKNOWN /* This is an unknown type */
#define HCNSE_FILE_TYPE_FILE     DT_REG     /* This is a regular file */
#define HCNSE_FILE_TYPE_DIR      DT_DIR     /* This is a directory */
#define HCNSE_FILE_TYPE_CHR      DT_CHR     /* This is a character device */
#define HCNSE_FILE_TYPE_BLK      DT_BLK     /* This is a block device */
#define HCNSE_FILE_TYPE_PIPE     DT_FIFO    /* This is a named pipe (FIFO) */
#define HCNSE_FILE_TYPE_LINK     DT_LNK     /* This is a symbolic link */
#define HCNSE_FILE_TYPE_SOCK     DT_SOCK    /* This is a UNIX domain socket */
#else
#define HCNSE_FILE_TYPE_UNKFILE  0          /* This is an unknown type */
#define HCNSE_FILE_TYPE_FILE     1          /* This is a regular file */
#define HCNSE_FILE_TYPE_DIR      2          /* This is a directory */
#define HCNSE_FILE_TYPE_CHR      3          /* This is a character device */
#define HCNSE_FILE_TYPE_BLK      4          /* This is a block device */
#define HCNSE_FILE_TYPE_PIPE     5          /* This is a named pipe (FIFO) */
#define HCNSE_FILE_TYPE_LINK     6          /* This is a symbolic link */
#define HCNSE_FILE_TYPE_SOCK     7          /* This is a UNIX domain socket */
#endif

struct hcnse_dir_s {
    DIR *dir;
    struct dirent *dirent;
    const char *name;
    uint8_t type;
};

struct hcnse_glob_s {
    glob_t glob;
    const char *pattern;
    size_t last;
};


#define hcnse_file_change_access(name, access) \
    chmod((const char *) name, access)

#define hcnse_file_rename(from, to) \
    rename((const char *) from, (const char *) to)

#define hcnse_file_delete(name)             unlink((const char *) name)
#define hcnse_dir_create(name, access)      mkdir((const char *) name, access)
#define hcnse_dir_delete(name)              rmdir((const char *) name)

#define hcnse_file_stat_by_path(st, path)   stat((const char *) path, st)
#define hcnse_file_stat_by_fd(st, fd)       fstat(fd, st)
#define hcnse_file_stat_by_link(st, path)   lstat((const char *) path, st)

#define hcnse_is_path_separator(c)          ((c) == '/')


#elif (HCNSE_WIN32)

/* Mode */
#define HCNSE_FILE_RDONLY                   GENERIC_READ
#define HCNSE_FILE_WRONLY                   GENERIC_WRITE
#define HCNSE_FILE_RDWR                     GENERIC_READ|GENERIC_WRITE

/* Create */
#define HCNSE_FILE_CREATE_OR_OPEN           OPEN_ALWAYS
#define HCNSE_FILE_OPEN                     OPEN_EXISTING
#define HCNSE_FILE_TRUNCATE                 CREATE_ALWAYS
#define HCNSE_FILE_APPEND                   FILE_APPEND_DATA|SYNCHRONIZE
#define HCNSE_FILE_CREATE_OR_APPEND         OPEN_ALWAYS /* fixme */
#define HCNSE_FILE_NONBLOCK                 0

/* Access */
#define HCNSE_FILE_DEFAULT_ACCESS           0
#define HCNSE_FILE_OWNER_ACCESS             0

#define HCNSE_INVALID_FILE                  INVALID_HANDLE_VALUE
#define HCNSE_FILE_ERROR                    0

#define HCNSE_STDIN                         GetStdHandle(STD_INPUT_HANDLE)
#define HCNSE_STDOUT                        GetStdHandle(STD_OUTPUT_HANDLE)
#define HCNSE_STDERR                        GetStdHandle(STD_ERROR_HANDLE)

#define HCNSE_PATH_SEPARATOR_STR            "\\"
#define HCNSE_PATH_SEPARATOR                '\\'

#define HCNSE_MAX_PATH_LEN                  MAX_PATH

#define HCNSE_FILE_TYPE_UNKFILE  0          /* This is an unknown type */
#define HCNSE_FILE_TYPE_FILE     1          /* This is a regular file */
#define HCNSE_FILE_TYPE_DIR      2          /* This is a directory */
#define HCNSE_FILE_TYPE_CHR      3          /* This is a character device */
#define HCNSE_FILE_TYPE_BLK      4          /* This is a block device */
#define HCNSE_FILE_TYPE_PIPE     5          /* This is a named pipe (FIFO) */
#define HCNSE_FILE_TYPE_LINK     6          /* This is a symbolic link */
#define HCNSE_FILE_TYPE_SOCK     7          /* This is a UNIX domain socket */

struct hcnse_dir_s {
    HANDLE dir;
    WIN32_FIND_DATA finddata;
    const char *name;
    uint8_t type;
};

struct hcnse_glob_s {
    HANDLE dir;
    WIN32_FIND_DATA finddata;
    const char *pattern;
    bool done;
    const char *last_res;
    size_t prefix_len;
};

#define hcnse_file_rename(from, to) \
    MoveFile((const char *) from, (const char *) to)

#define hcnse_file_delete(name)             DeleteFile((const char *) name)

#define hcnse_file_stat_by_fd(fd, fi)       GetFileInformationByHandle(fd, fi)
#define hcnse_file_stat_by_link(name, fi)   hcnse_file_stat_by_path(fi, name)

#define hcnse_is_path_separator(c)          ((c) == '\\')

hcnse_err_t hcnse_file_stat_by_path(hcnse_file_stat_t *stat, const char *path);

#endif

#define hcnse_dir_current_namelen(dir) \
    hcnse_strlen(hcnse_dir_current_name(dir))

struct hcnse_file_s {
    hcnse_fd_t fd;
    char *name;
    hcnse_off_t offset;
};

struct hcnse_file_info_s {
    hcnse_file_type_t type;
    hcnse_file_access_t access;
    hcnse_file_stat_t stat;
};


hcnse_err_t hcnse_file_open(hcnse_file_t *file, const char *path, hcnse_uint_t mode,
    hcnse_uint_t create, hcnse_uint_t access);
ssize_t hcnse_file_read(hcnse_file_t *file, uint8_t *buf, size_t size,
    hcnse_off_t offset);
ssize_t hcnse_file_write(hcnse_file_t *file, const char *buf, size_t size,
    hcnse_off_t offset);
ssize_t hcnse_file_size(hcnse_file_t *file);
void hcnse_file_close(hcnse_file_t *file);

ssize_t hcnse_read_fd(hcnse_fd_t fd, void *buf, size_t n);
ssize_t hcnse_write_fd(hcnse_fd_t fd, void *buf, size_t n);

ssize_t hcnse_write_stdout(const char *str);
ssize_t hcnse_write_stderr(const char *str);

hcnse_err_t hcnse_file_info_by_path(hcnse_file_info_t *info, const char *path);
hcnse_err_t hcnse_file_info(hcnse_file_info_t *info, hcnse_file_t *file);

hcnse_err_t hcnse_dir_open(hcnse_dir_t *dir, const char *path);
hcnse_err_t hcnse_dir_read(hcnse_dir_t *dir);
const char *hcnse_dir_name(hcnse_dir_t *dir);
const char *hcnse_dir_current_name(hcnse_dir_t *dir);
hcnse_file_type_t hcnse_dir_current_file_type(hcnse_dir_t *dir);
hcnse_err_t hcnse_dir_current_is_file(hcnse_dir_t *dir);
void hcnse_dir_close(hcnse_dir_t *dir);

int hcnse_glob_open(hcnse_glob_t *gl, const char *pattern);
int hcnse_glob_read(hcnse_glob_t *gl, char *res);
void hcnse_glob_close(hcnse_glob_t *gl);

size_t hcnse_file_full_path(char *buf, const char *path, const char *file);
bool hcnse_is_path_has_wildcard(const char *path);
hcnse_err_t hcnse_check_absolute_path(const char *path);

#endif /* INCLUDED_HCNSE_FILESYS_H */
