#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* ── POSIX types ── */
typedef int32_t pid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef int64_t off_t;
typedef uint64_t ino_t;
typedef uint32_t mode_t;
typedef uint32_t dev_t;
typedef uint32_t nlink_t;
typedef int64_t blksize_t;
typedef int64_t blkcnt_t;
typedef int64_t time_t;
typedef long ssize_t;

/* ── file types (S_IFMT) ── */
#define S_IFMT   0170000
#define S_IFREG  0100000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFBLK  0060000
#define S_IFIFO  0010000
#define S_IFLNK  0120000
#define S_IFSOCK 0140000

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

/* ── file permissions ── */
#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 0070
#define S_IRGRP 0040
#define S_IWGRP 0020
#define S_IXGRP 0010
#define S_IRWXO 0007
#define S_IROTH 0004
#define S_IWOTH 0002
#define S_IXOTH 0001

/* ── open flags (fcntl.h) ── */
#define O_RDONLY   0
#define O_WRONLY   1
#define O_RDWR     2
#define O_APPEND   0x0008
#define O_CREAT    0x0100
#define O_EXCL     0x0200
#define O_TRUNC    0x1000
#define O_NONBLOCK 0x0800
#define O_NOCTTY   0x0400
#define O_SYNC     0x2000
#define O_ASYNC    0x4000
#define O_DIRECT   0x8000
#define O_LARGEFILE 0x10000
#define O_DIRECTORY 0x20000
#define O_NOFOLLOW  0x40000
#define O_CLOEXEC   0x80000

/* ── seek whence (unistd.h) ── */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* ── access modes ── */
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

/* ── stat structure (sys/stat.h) ── */
typedef struct {
    dev_t     st_dev;
    ino_t     st_ino;
    mode_t    st_mode;
    nlink_t   st_nlink;
    uid_t     st_uid;
    gid_t     st_gid;
    dev_t     st_rdev;
    off_t     st_size;
    blksize_t st_blksize;
    blkcnt_t  st_blocks;
    time_t    st_atime;
    time_t    st_mtime;
    time_t    st_ctime;
} stat_t;

/* ── dirent structure (dirent.h) ── */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12

typedef struct {
    ino_t     d_ino;
    off_t     d_off;
    uint16_t  d_reclen;
    uint8_t   d_type;
    char      d_name[256];
} dirent_t;

typedef struct {
    int fd;
    dirent_t *entries;
    int count;
    int pos;
} DIR;

/* ── file descriptor operations ── */
typedef struct file file_t;

typedef struct {
    ssize_t (*read)(file_t *file, void *buf, size_t count);
    ssize_t (*write)(file_t *file, const void *buf, size_t count);
    off_t (*lseek)(file_t *file, off_t offset, int whence);
    int (*ioctl)(file_t *file, unsigned long cmd, void *arg);
    int (*fcntl)(file_t *file, int cmd, void *arg);
    int (*fsync)(file_t *file);
    int (*fstat)(file_t *file, stat_t *stat);
    int (*close)(file_t *file);
} file_ops_t;

/* ── file structure ── */
struct file {
    int f_fd;
    int f_flags;
    off_t f_pos;
    mode_t f_mode;
    ino_t f_ino;
    dev_t f_dev;
    void *f_private;
    file_ops_t *f_op;
};

/* ── POSIX file I/O API ── */
int open(const char *path, int oflag, ...);
int creat(const char *path, mode_t mode);
int close(int fd);
void vfs_init(void);
ssize_t read(int fd, void *buf, size_t nbyte);
ssize_t write(int fd, const void *buf, size_t nbyte);
off_t lseek(int fd, off_t offset, int whence);
int dup(int fd);
int dup2(int fd, int fd2);
int fcntl(int fd, int cmd, ...);
int ioctl(int fd, unsigned long cmd, ...);
int fsync(int fd);
int fstat(int fd, stat_t *buf);
int stat(const char *path, stat_t *buf);
int lstat(const char *path, stat_t *buf);
int access(const char *path, int amode);

/* ── POSIX directory API ── */
int mkdir(const char *path, mode_t mode);
int rmdir(const char *path);
DIR *opendir(const char *path);
dirent_t *readdir(DIR *dirp);
int closedir(DIR *dirp);
int chdir(const char *path);
char *getcwd(char *buf, size_t size);

/* ── POSIX file manipulation ── */
int unlink(const char *path);
int rename(const char *old, const char *new);
int link(const char *path1, const char *path2);
int symlink(const char *path1, const char *path2);
ssize_t readlink(const char *path, char *buf, size_t bufsiz);
int chmod(const char *path, mode_t mode);
int fchmod(int fd, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *path, uid_t owner, gid_t group);

/* ── POSIX pipe/fifo ── */
int pipe(int fd[2]);
int mkfifo(const char *path, mode_t mode);

/* ── POSIX file locking ── */
#define F_GETLK  5
#define F_SETLK  6
#define F_SETLKW 7

typedef struct {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
} flock_t;

int lockf(int fd, int function, off_t size);

/* ── POSIX select/poll ── */
typedef struct {
    int fd;
    short events;
    short revents;
} pollfd_t;

#define POLLIN   0x0001
#define POLLOUT  0x0004
#define POLLERR  0x0008
#define POLLHUP  0x0010
#define POLLNVAL 0x0020

int poll(pollfd_t *fds, int nfds, int timeout);

#endif
