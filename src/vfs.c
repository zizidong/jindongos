#include "vfs.h"
#include "libc.h"
#include "syscall.h"

#define MAX_OPEN_FILES 256
#define MAX_DIRS 64
#define MAX_ENTRIES 1024

/* ── file descriptor table ── */
static file_t fd_table[MAX_OPEN_FILES];

/* ── directory entries (ramfs) ── */
typedef struct {
    char name[256];
    mode_t mode;
    off_t size;
    void *data;
    ino_t ino;
    time_t mtime;
} vfs_entry_t;

static vfs_entry_t entries[MAX_ENTRIES];
static int entry_count = 0;
static ino_t next_ino = 1;

/* ── current working directory ── */
static char cwd[256] = "/";

/* ── allocate file descriptor ── */
static int alloc_fd(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (fd_table[i].f_fd == -1) return i;
    }
    return -1;
}

/* ── find entry by path ── */
static vfs_entry_t *find_entry(const char *path) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, path) == 0) {
            return &entries[i];
        }
    }
    return NULL;
}

/* ── allocate entry ── */
static vfs_entry_t *alloc_entry(const char *name, mode_t mode) {
    if (entry_count >= MAX_ENTRIES) return NULL;
    vfs_entry_t *e = &entries[entry_count++];
    strcpy(e->name, name);
    e->mode = mode;
    e->size = 0;
    e->data = NULL;
    e->ino = next_ino++;
    e->mtime = 0;
    return e;
}

/* ── POSIX open ── */
int open(const char *path, int oflag, ...) {
    va_list ap;
    va_start(ap, oflag);
    mode_t mode = va_arg(ap, mode_t);
    va_end(ap);
    
    vfs_entry_t *entry = find_entry(path);
    
    if (!entry) {
        if (!(oflag & O_CREAT)) return -1;
        entry = alloc_entry(path, S_IFREG | mode);
        if (!entry) return -1;
    }
    
    int fd = alloc_fd();
    if (fd < 0) return -1;
    
    file_t *f = &fd_table[fd];
    f->f_fd = fd;
    f->f_flags = oflag;
    f->f_pos = (oflag & O_APPEND) ? entry->size : 0;
    f->f_mode = entry->mode;
    f->f_ino = entry->ino;
    f->f_dev = 0;
    f->f_private = entry;
    
    return fd;
}

/* ── POSIX creat ── */
int creat(const char *path, mode_t mode) {
    return open(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

/* ── POSIX close ── */
int close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    fd_table[fd].f_fd = -1;
    return 0;
}

/* ── POSIX read ── */
ssize_t read(int fd, void *buf, size_t nbyte) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    file_t *f = &fd_table[fd];
    vfs_entry_t *e = (vfs_entry_t *)f->f_private;
    
    if (!e || !e->data) return 0;
    
    size_t to_read = nbyte;
    if (f->f_pos + (off_t)to_read > (off_t)e->size) {
        to_read = (size_t)(e->size - f->f_pos);
    }
    
    if (to_read > 0) {
        memcpy(buf, (uint8_t *)e->data + f->f_pos, to_read);
        f->f_pos += to_read;
    }
    
    return (ssize_t)to_read;
}

/* ── POSIX write ── */
ssize_t write(int fd, const void *buf, size_t nbyte) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    file_t *f = &fd_table[fd];
    vfs_entry_t *e = (vfs_entry_t *)f->f_private;
    
    if (!e) return -1;
    
    /* allocate/reallocate data buffer */
    off_t new_size = f->f_pos + (off_t)nbyte;
    if (new_size > (off_t)e->size) {
        void *new_data = malloc(new_size);
        if (!new_data) return -1;
        
        if (e->data && e->size > 0) {
            memcpy(new_data, e->data, e->size);
        }
        e->data = new_data;
        e->size = new_size;
    }
    
    memcpy((uint8_t *)e->data + f->f_pos, buf, nbyte);
    f->f_pos += nbyte;
    
    return (ssize_t)nbyte;
}

/* ── POSIX lseek ── */
off_t lseek(int fd, off_t offset, int whence) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    file_t *f = &fd_table[fd];
    vfs_entry_t *e = (vfs_entry_t *)f->f_private;
    
    off_t new_pos = 0;
    switch (whence) {
    case SEEK_SET:
        new_pos = offset;
        break;
    case SEEK_CUR:
        new_pos = f->f_pos + offset;
        break;
    case SEEK_END:
        new_pos = (off_t)e->size + offset;
        break;
    default:
        return -1;
    }
    
    if (new_pos < 0) return -1;
    f->f_pos = new_pos;
    return new_pos;
}

/* ── POSIX dup ── */
int dup(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    int new_fd = alloc_fd();
    if (new_fd < 0) return -1;
    
    fd_table[new_fd] = fd_table[fd];
    fd_table[new_fd].f_fd = new_fd;
    
    return new_fd;
}

/* ── POSIX dup2 ── */
int dup2(int fd, int fd2) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    if (fd2 < 0 || fd2 >= MAX_OPEN_FILES) return -1;
    
    if (fd_table[fd2].f_fd != -1) close(fd2);
    
    fd_table[fd2] = fd_table[fd];
    fd_table[fd2].f_fd = fd2;
    
    return fd2;
}

/* ── POSIX fstat ── */
int fstat(int fd, stat_t *buf) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    if (!buf) return -1;
    
    file_t *f = &fd_table[fd];
    vfs_entry_t *e = (vfs_entry_t *)f->f_private;
    
    if (!e) return -1;
    
    buf->st_dev = 0;
    buf->st_ino = e->ino;
    buf->st_mode = e->mode;
    buf->st_nlink = 1;
    buf->st_uid = 0;
    buf->st_gid = 0;
    buf->st_rdev = 0;
    buf->st_size = e->size;
    buf->st_blksize = 4096;
    buf->st_blocks = (e->size + 511) / 512;
    buf->st_atime = e->mtime;
    buf->st_mtime = e->mtime;
    buf->st_ctime = e->mtime;
    
    return 0;
}

/* ── POSIX stat ── */
int stat(const char *path, stat_t *buf) {
    if (!buf) return -1;
    
    vfs_entry_t *e = find_entry(path);
    if (!e) return -1;
    
    buf->st_dev = 0;
    buf->st_ino = e->ino;
    buf->st_mode = e->mode;
    buf->st_nlink = 1;
    buf->st_uid = 0;
    buf->st_gid = 0;
    buf->st_rdev = 0;
    buf->st_size = e->size;
    buf->st_blksize = 4096;
    buf->st_blocks = (e->size + 511) / 512;
    buf->st_atime = e->mtime;
    buf->st_mtime = e->mtime;
    buf->st_ctime = e->mtime;
    
    return 0;
}

/* ── POSIX lstat ── */
int lstat(const char *path, stat_t *buf) {
    return stat(path, buf);
}

/* ── POSIX access ── */
int access(const char *path, int amode) {
    vfs_entry_t *e = find_entry(path);
    if (!e) return -1;
    
    (void)amode;
    return 0;
}

/* ── POSIX mkdir ── */
int mkdir(const char *path, mode_t mode) {
    vfs_entry_t *e = find_entry(path);
    if (e) return -1;
    
    e = alloc_entry(path, S_IFDIR | mode);
    return e ? 0 : -1;
}

/* ── POSIX rmdir ── */
int rmdir(const char *path) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, path) == 0) {
            if (!S_ISDIR(entries[i].mode)) return -1;
            for (int j = i; j < entry_count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            entry_count--;
            return 0;
        }
    }
    return -1;
}

/* ── POSIX unlink ── */
int unlink(const char *path) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, path) == 0) {
            if (S_ISDIR(entries[i].mode)) return -1;
            for (int j = i; j < entry_count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            entry_count--;
            return 0;
        }
    }
    return -1;
}

/* ── POSIX rename ── */
int rename(const char *old, const char *new) {
    vfs_entry_t *e = find_entry(old);
    if (!e) return -1;
    
    strcpy(e->name, new);
    return 0;
}

/* ── POSIX chdir ── */
int chdir(const char *path) {
    vfs_entry_t *e = find_entry(path);
    if (!e || !S_ISDIR(e->mode)) return -1;
    
    strcpy(cwd, path);
    return 0;
}

/* ── POSIX getcwd ── */
char *getcwd(char *buf, size_t size) {
    if (!buf || size == 0) return NULL;
    strncpy(buf, cwd, size - 1);
    buf[size - 1] = 0;
    return buf;
}

/* ── POSIX opendir ── */
DIR *opendir(const char *path) {
    vfs_entry_t *e = find_entry(path);
    if (!e || !S_ISDIR(e->mode)) return NULL;
    
    DIR *dir = (DIR *)malloc(sizeof(DIR));
    if (!dir) return NULL;
    
    dir->fd = -1;
    dir->entries = NULL;
    dir->count = 0;
    dir->pos = 0;
    
    return dir;
}

/* ── POSIX readdir ── */
dirent_t *readdir(DIR *dirp) {
    if (!dirp) return NULL;
    
    static dirent_t de;
    if (dirp->pos >= entry_count) return NULL;
    
    vfs_entry_t *e = &entries[dirp->pos++];
    de.d_ino = e->ino;
    de.d_off = dirp->pos;
    de.d_reclen = sizeof(dirent_t);
    de.d_type = S_ISDIR(e->mode) ? DT_DIR : DT_REG;
    strcpy(de.d_name, e->name);
    
    return &de;
}

/* ── POSIX closedir ── */
int closedir(DIR *dirp) {
    if (dirp) free(dirp);
    return 0;
}

/* ── POSIX link ── */
int link(const char *path1, const char *path2) {
    vfs_entry_t *e = find_entry(path1);
    if (!e) return -1;
    
    vfs_entry_t *new_e = alloc_entry(path2, e->mode);
    if (!new_e) return -1;
    
    new_e->data = e->data;
    new_e->size = e->size;
    
    return 0;
}

/* ── POSIX symlink ── */
int symlink(const char *path1, const char *path2) {
    return link(path1, path2);
}

/* ── POSIX readlink ── */
ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
    vfs_entry_t *e = find_entry(path);
    if (!e) return -1;
    
    size_t len = strlen(e->name);
    if (len > bufsiz) len = bufsiz;
    
    memcpy(buf, e->name, len);
    return (ssize_t)len;
}

/* ── POSIX chmod ── */
int chmod(const char *path, mode_t mode) {
    vfs_entry_t *e = find_entry(path);
    if (!e) return -1;
    
    e->mode = (e->mode & S_IFMT) | (mode & ~S_IFMT);
    return 0;
}

/* ── POSIX fchmod ── */
int fchmod(int fd, mode_t mode) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (fd_table[fd].f_fd == -1) return -1;
    
    vfs_entry_t *e = (vfs_entry_t *)fd_table[fd].f_private;
    if (!e) return -1;
    
    e->mode = (e->mode & S_IFMT) | (mode & ~S_IFMT);
    return 0;
}

/* ── POSIX chown ── */
int chown(const char *path, uid_t owner, gid_t group) {
    (void)path; (void)owner; (void)group;
    return 0;
}

/* ── POSIX fchown ── */
int fchown(int fd, uid_t owner, gid_t group) {
    (void)fd; (void)owner; (void)group;
    return 0;
}

/* ── POSIX lchown ── */
int lchown(const char *path, uid_t owner, gid_t group) {
    (void)path; (void)owner; (void)group;
    return 0;
}

/* ── POSIX pipe ── */
int pipe(int fd[2]) {
    (void)fd;
    return -1;
}

/* ── POSIX mkfifo ── */
int mkfifo(const char *path, mode_t mode) {
    (void)path; (void)mode;
    return -1;
}

/* ── POSIX fcntl ── */
int fcntl(int fd, int cmd, ...) {
    (void)fd; (void)cmd;
    return -1;
}

/* ── POSIX ioctl ── */
int ioctl(int fd, unsigned long cmd, ...) {
    (void)fd; (void)cmd;
    return -1;
}

/* ── POSIX fsync ── */
int fsync(int fd) {
    (void)fd;
    return 0;
}

/* ── POSIX lockf ── */
int lockf(int fd, int function, off_t size) {
    (void)fd; (void)function; (void)size;
    return 0;
}

/* ── POSIX poll ── */
int poll(pollfd_t *fds, int nfds, int timeout) {
    (void)fds; (void)nfds; (void)timeout;
    return 0;
}

/* ── VFS initialization ── */
void vfs_init(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        fd_table[i].f_fd = -1;
    }
    
    /* create root directory */
    alloc_entry("/", S_IFDIR | 0755);
}
