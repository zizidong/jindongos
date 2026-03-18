#ifndef _LIBC_H
#define _LIBC_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* ── C23 _Bool ── */
#ifndef __cplusplus
#define true 1
#define false 0
#endif

/* ── errno ── */
extern int errno;
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32

/* ── C23 _BitInt ── */
#define _BitInt(N) int64_t

/* ── POSIX types ── */
typedef int pid_t;
typedef long ssize_t;

/* ── string.h (C23) ── */
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
char *strcat(char *dst, const char *src);
char *strncat(char *dst, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int strcoll(const char *s1, const char *s2);
size_t strxfrm(char *dst, const char *src, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);
char *strpbrk(const char *s, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);
char *strerror(int errnum);
char *strerror_r(int errnum, char *buf, size_t buflen);
char *strdup(const char *s);
char *strndup(const char *s, size_t n);

/* ── memory.h (C23) ── */
void *memset(void *s, int c, size_t n);
void *memset_explicit(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void *memcpy_s(void *dst, size_t dstsize, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);

/* ── ctype.h (C23) ── */
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);
int isspace(int c);
int isupper(int c);
int islower(int c);
int isprint(int c);
int isgraph(int c);
int iscntrl(int c);
int ispunct(int c);
int isxdigit(int c);
int isblank(int c);
int toupper(int c);
int tolower(int c);

/* ── stdlib.h (C23) ── */
int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);
long strtol(const char *nptr, char **endptr, int base);
long long strtoll(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *nptr, char **endptr, int base);
double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *aligned_alloc(size_t alignment, size_t size);

/* heap init - called by kernel with Limine memmap */
extern uint8_t *_heap_base;
extern uint64_t _heap_size;
extern uint64_t _heap_used;

int abs(int j);
long labs(long j);
long long llabs(long long j);

typedef struct { int quot; int rem; } div_t;
typedef struct { long quot; long rem; } ldiv_t;
typedef struct { long long quot; long long rem; } lldiv_t;

div_t div(int numer, int denom);
ldiv_t ldiv(long numer, long denom);
lldiv_t lldiv(long long numer, long long denom);

int rand(void);
void srand(unsigned int seed);

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void qsort_s(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void *bsearch_s(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);

int system(const char *string);
char *getenv(const char *name);
int putenv(char *string);
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);

/* ── stdio.h (C23) ── */
typedef struct {
    int fd;
    int flags;
    char *buf;
    size_t buf_size;
    size_t buf_pos;
} FILE;

typedef long fpos_t;

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define BUFSIZ 8192
#define FILENAME_MAX 256
#define FOPEN_MAX 256
#define L_tmpnam 256

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE *fopen(const char *path, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);
int fclose(FILE *stream);
int fflush(FILE *stream);

int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *s);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
int ungetc(int c, FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, const fpos_t *pos);

int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vprintf(const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int fscanf(FILE *stream, const char *format, ...);
int scanf(const char *format, ...);
int sscanf(const char *s, const char *format, ...);

int remove(const char *path);
int rename(const char *old, const char *new);
FILE *tmpfile(void);
char *tmpnam(char *s);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);

/* ── math.h (C23) ── */
double sqrt(double x);
double cbrt(double x);
double pow(double x, double y);
double hypot(double x, double y);
double fabs(double x);
double floor(double x);
double ceil(double x);
double round(double x);
double trunc(double x);
double rint(double x);
double nearbyint(double x);
double fmod(double x, double y);
double remainder(double x, double y);
double fma(double x, double y, double z);

double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);

double sinh(double x);
double cosh(double x);
double tanh(double x);
double asinh(double x);
double acosh(double x);
double atanh(double x);

double exp(double x);
double exp2(double x);
double exp10(double x);
double expm1(double x);
double log(double x);
double log10(double x);
double log2(double x);
double log1p(double x);

double ldexp(double x, int exp);
double frexp(double x, int *exp);
double modf(double x, double *ipart);
double scalb(double x, double n);
double scalbln(double x, long n);

double copysign(double x, double y);
double nextafter(double x, double y);
double nexttoward(double x, long double y);
double fdim(double x, double y);
double fmax(double x, double y);
double fmin(double x, double y);

int isnan(double x);
int isinf(double x);
int isfinite(double x);
int isnormal(double x);
int signbit(double x);
int fpclassify(double x);

/* ── time.h (C23) ── */
typedef long time_t;
typedef long clock_t;

#define CLOCKS_PER_SEC 1000000

typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} tm_t;

typedef struct {
    time_t tv_sec;
    long tv_nsec;
} timespec_t;

time_t time(time_t *timer);
clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(tm_t *timeptr);
char *ctime(const time_t *timer);
char *asctime(const tm_t *timeptr);
tm_t *gmtime(const time_t *timer);
tm_t *localtime(const time_t *timer);
size_t strftime(char *s, size_t maxsize, const char *format, const tm_t *timeptr);
int timespec_get(timespec_t *ts, int base);

/* ── assert.h (C23) ── */
void assert(int expression);
void static_assert(_Bool condition, const char *message);

/* ── setjmp.h (C23) ── */
typedef int jmp_buf[32];

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

/* ── signal.h (C23) ── */
typedef void (*sig_t)(int);

#define SIGABRT 6
#define SIGFPE 8
#define SIGILL 4
#define SIGINT 2
#define SIGSEGV 11
#define SIGTERM 15

sig_t signal(int sig, sig_t func);
int raise(int sig);

/* ── C23 _Noreturn ── */
#define _Noreturn

/* ── C23 _Generic ── */
#define _Generic(x, ...) (x)

/* ── C23 typeof ── */
#define typeof(x) __typeof__(x)

/* ── C23 _Alignof ── */
#define _Alignof(T) __alignof__(T)

/* ── C23 _Alignas ── */
#define _Alignas(N) __attribute__((aligned(N)))

/* ── C23 _Static_assert ── */
#define _Static_assert(cond, msg) static_assert(cond, msg)

/* ── C23 _Noreturn functions ── */
_Noreturn void exit(int status);
_Noreturn void abort(void);

#endif
