#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>
#include <stddef.h>

/* ── POSIX types ── */
typedef int pid_t;
typedef long ssize_t;

/* ── syscall numbers ── */
#define SYS_exit        0
#define SYS_write       1
#define SYS_read        2
#define SYS_open        3
#define SYS_close       4
#define SYS_stat        5
#define SYS_fstat       6
#define SYS_lstat       7
#define SYS_poll        8
#define SYS_lseek       9
#define SYS_mmap        10
#define SYS_mprotect    11
#define SYS_munmap      12
#define SYS_brk         13
#define SYS_rt_sigaction 14
#define SYS_rt_sigprocmask 15
#define SYS_rt_sigpending 16
#define SYS_rt_sigtimedwait 17
#define SYS_rt_sigqueueinfo 18
#define SYS_rt_sigreturn 19
#define SYS_ioctl       20
#define SYS_pread64     21
#define SYS_pwrite64    22
#define SYS_readv       23
#define SYS_writev      24
#define SYS_access      25
#define SYS_pipe        26
#define SYS_select      27
#define SYS_sched_yield 28
#define SYS_mremap      29
#define SYS_msync       30
#define SYS_mincore     31
#define SYS_madvise     32
#define SYS_shmget      33
#define SYS_shmat       34
#define SYS_shmctl      35
#define SYS_dup         36
#define SYS_dup2        37
#define SYS_pause       38
#define SYS_nanosleep   39
#define SYS_getitimer   40
#define SYS_alarm       41
#define SYS_setitimer   42
#define SYS_getpid      43
#define SYS_sendfile    44
#define SYS_socket      45
#define SYS_connect     46
#define SYS_accept      47
#define SYS_sendto      48
#define SYS_recvfrom    49
#define SYS_sendmsg     50
#define SYS_recvmsg     51
#define SYS_shutdown    52
#define SYS_bind        53
#define SYS_listen      54
#define SYS_getsockname 55
#define SYS_getpeername 56
#define SYS_socketpair  57
#define SYS_setsockopt  58
#define SYS_getsockopt  59
#define SYS_clone       60
#define SYS_fork        61
#define SYS_vfork       62
#define SYS_execve      63
#define SYS_exit_group  64
#define SYS_wait4       65
#define SYS_kill        66
#define SYS_uname       67
#define SYS_fcntl       68
#define SYS_flock       69
#define SYS_fsync       70
#define SYS_fdatasync   71
#define SYS_truncate    72
#define SYS_ftruncate   73
#define SYS_getdents    74
#define SYS_getcwd      75
#define SYS_chdir       76
#define SYS_fchdir      77
#define SYS_rename      78
#define SYS_mkdir       79
#define SYS_rmdir       80
#define SYS_creat       81
#define SYS_link        82
#define SYS_unlink      83
#define SYS_symlink     84
#define SYS_readlink    85
#define SYS_chmod       86
#define SYS_fchmod      87
#define SYS_chown       88
#define SYS_fchown      89
#define SYS_lchown      90
#define SYS_umask       91
#define SYS_gettimeofday 92
#define SYS_getrlimit   93
#define SYS_getrusage   94
#define SYS_sysinfo     95
#define SYS_times       96
#define SYS_ptrace      97
#define SYS_getuid      98
#define SYS_syslog      99
#define SYS_getgid      100

/* ── file descriptor ── */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* ── syscall interface ── */
long syscall(long number, ...);

/* ── wrapper functions ── */
void sys_exit(int status);
ssize_t sys_write(int fd, const void *buf, size_t count);
ssize_t sys_read(int fd, void *buf, size_t count);
int sys_open(const char *pathname, int flags, int mode);
int sys_close(int fd);
pid_t sys_getpid(void);
int sys_fork(void);
int sys_execve(const char *filename, char *const argv[], char *const envp[]);

#endif
