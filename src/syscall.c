#include "syscall.h"
#include "libc.h"

/* ── simple file descriptor table ── */
#define MAX_FDS 256

typedef struct {
    uint8_t used;
    uint32_t type;  /* 0=unused, 1=stdout, 2=stderr, 3=file */
    uint32_t offset;
} fd_entry_t;

static fd_entry_t fd_table[MAX_FDS];
static uint32_t next_fd = 3;

/* ── process table ── */
#define MAX_PROCESSES 16

typedef struct {
    uint32_t pid;
    uint8_t used;
    uint8_t running;
} process_t;

static process_t process_table[MAX_PROCESSES];
static uint32_t next_pid = 1;

void syscall_init(void) {
    /* initialize stdout/stderr */
    fd_table[STDOUT_FILENO].used = 1;
    fd_table[STDOUT_FILENO].type = 1;
    fd_table[STDERR_FILENO].used = 1;
    fd_table[STDERR_FILENO].type = 2;
}

/* ── syscall dispatcher ── */
long syscall_handler(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    (void)arg4; (void)arg5; (void)arg6;  /* unused parameters */
    switch (number) {
    case SYS_exit:
        for (;;) __asm__("hlt");
        return 0;
    
    case SYS_write: {
        int fd = (int)arg1;
        const void *buf = (const void *)arg2;
        size_t count = (size_t)arg3;
        (void)buf;  /* unused */
        
        if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
            /* write to framebuffer (stub) */
            return (long)count;
        }
        return -1;
    }
    
    case SYS_read: {
        int fd = (int)arg1;
        void *buf = (void *)arg2;
        size_t count = (size_t)arg3;
        (void)buf; (void)count;  /* unused */
        
        if (fd == STDIN_FILENO) {
            /* read from keyboard (stub) */
            return 0;
        }
        return -1;
    }
    
    case SYS_open: {
        const char *pathname = (const char *)arg1;
        int flags = (int)arg2;
        int mode = (int)arg3;
        
        (void)pathname; (void)flags; (void)mode;
        
        if (next_fd >= MAX_FDS) return -1;
        int fd = next_fd++;
        fd_table[fd].used = 1;
        fd_table[fd].type = 3;
        fd_table[fd].offset = 0;
        return fd;
    }
    
    case SYS_close: {
        int fd = (int)arg1;
        if (fd < 0 || fd >= MAX_FDS) return -1;
        fd_table[fd].used = 0;
        return 0;
    }
    
    case SYS_lseek: {
        int fd = (int)arg1;
        long offset = (long)arg2;
        int whence = (int)arg3;
        
        if (fd < 0 || fd >= MAX_FDS || !fd_table[fd].used) return -1;
        
        if (whence == 0) fd_table[fd].offset = offset;
        else if (whence == 1) fd_table[fd].offset += offset;
        
        return (long)fd_table[fd].offset;
    }
    
    case SYS_getpid: {
        return 1;  /* always return PID 1 */
    }
    
    case SYS_fork: {
        if (next_pid >= MAX_PROCESSES) return -1;
        uint32_t pid = next_pid++;
        process_table[pid].pid = pid;
        process_table[pid].used = 1;
        process_table[pid].running = 1;
        return (long)pid;
    }
    
    case SYS_exit_group: {
        int status = (int)arg1;
        (void)status;
        for (;;) __asm__("hlt");
        return 0;
    }
    
    case SYS_brk: {
        /* memory management - stub */
        return (long)arg1;
    }
    
    case SYS_mmap: {
        /* memory mapping - stub */
        return (long)arg1;
    }
    
    case SYS_munmap: {
        /* memory unmapping - stub */
        return 0;
    }
    
    case SYS_mprotect: {
        /* memory protection - stub */
        return 0;
    }
    
    case SYS_rt_sigaction:
    case SYS_rt_sigprocmask:
    case SYS_rt_sigpending:
        /* signal handling - stub */
        return 0;
    
    case SYS_ioctl: {
        int fd = (int)arg1;
        unsigned long request = (unsigned long)arg2;
        void *argp = (void *)arg3;
        
        (void)fd; (void)request; (void)argp;
        return 0;
    }
    
    case SYS_fcntl: {
        int fd = (int)arg1;
        int cmd = (int)arg2;
        long arg = (long)arg3;
        
        (void)fd; (void)cmd; (void)arg;
        return 0;
    }
    
    case SYS_dup: {
        int oldfd = (int)arg1;
        if (oldfd < 0 || oldfd >= MAX_FDS || !fd_table[oldfd].used) return -1;
        if (next_fd >= MAX_FDS) return -1;
        int newfd = next_fd++;
        fd_table[newfd] = fd_table[oldfd];
        return newfd;
    }
    
    case SYS_dup2: {
        int oldfd = (int)arg1;
        int newfd = (int)arg2;
        if (oldfd < 0 || oldfd >= MAX_FDS || !fd_table[oldfd].used) return -1;
        if (newfd < 0 || newfd >= MAX_FDS) return -1;
        fd_table[newfd] = fd_table[oldfd];
        return newfd;
    }
    
    case SYS_pipe: {
        int *pipefd = (int *)arg1;
        if (!pipefd) return -1;
        pipefd[0] = next_fd++;
        pipefd[1] = next_fd++;
        fd_table[pipefd[0]].used = 1;
        fd_table[pipefd[1]].used = 1;
        return 0;
    }
    
    case SYS_getcwd: {
        char *buf = (char *)arg1;
        size_t size = (size_t)arg2;
        if (buf && size > 0) {
            strcpy(buf, "/");
        }
        return (long)buf;
    }
    
    case SYS_chdir: {
        const char *path = (const char *)arg1;
        (void)path;
        return 0;
    }
    
    case SYS_mkdir: {
        const char *pathname = (const char *)arg1;
        int mode = (int)arg2;
        (void)pathname; (void)mode;
        return 0;
    }
    
    case SYS_rmdir: {
        const char *pathname = (const char *)arg1;
        (void)pathname;
        return 0;
    }
    
    case SYS_unlink: {
        const char *pathname = (const char *)arg1;
        (void)pathname;
        return 0;
    }
    
    case SYS_gettimeofday: {
        /* time - stub */
        return 0;
    }
    
    case SYS_getuid:
    case SYS_getgid:
        return 1000;  /* non-root user */
    
    case SYS_sched_yield:
        return 0;
    
    default:
        return -1;
    }
}

/* ── wrapper functions ── */
void sys_exit(int status) {
    syscall_handler(SYS_exit, status, 0, 0, 0, 0, 0);
}

ssize_t sys_write(int fd, const void *buf, size_t count) {
    return (ssize_t)syscall_handler(SYS_write, fd, (long)buf, count, 0, 0, 0);
}

ssize_t sys_read(int fd, void *buf, size_t count) {
    return (ssize_t)syscall_handler(SYS_read, fd, (long)buf, count, 0, 0, 0);
}

int sys_open(const char *pathname, int flags, int mode) {
    return (int)syscall_handler(SYS_open, (long)pathname, flags, mode, 0, 0, 0);
}

int sys_close(int fd) {
    return (int)syscall_handler(SYS_close, fd, 0, 0, 0, 0, 0);
}

pid_t sys_getpid(void) {
    return (pid_t)syscall_handler(SYS_getpid, 0, 0, 0, 0, 0, 0);
}

int sys_fork(void) {
    return (int)syscall_handler(SYS_fork, 0, 0, 0, 0, 0, 0);
}

int sys_execve(const char *filename, char *const argv[], char *const envp[]) {
    return (int)syscall_handler(SYS_execve, (long)filename, (long)argv, (long)envp, 0, 0, 0);
}

/* ── generic syscall (for user programs) ── */
long syscall(long number, ...) {
    va_list ap;
    va_start(ap, number);
    long arg1 = va_arg(ap, long);
    long arg2 = va_arg(ap, long);
    long arg3 = va_arg(ap, long);
    long arg4 = va_arg(ap, long);
    long arg5 = va_arg(ap, long);
    long arg6 = va_arg(ap, long);
    va_end(ap);
    
    return syscall_handler(number, arg1, arg2, arg3, arg4, arg5, arg6);
}
