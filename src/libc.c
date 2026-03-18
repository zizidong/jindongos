#include "libc.h"

int errno = 0;

/* string */
size_t strlen(const char *s) {
    size_t n = 0;
    while (s && s[n]) n++;
    return n;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t n = 0;
    while (n < maxlen && s && s[n]) n++;
    return n;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while (src && *src) *d++ = *src++;
    *d = 0;
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t i = 0;
    while (i < n && src && src[i]) {
        dst[i] = src[i];
        i++;
    }
    while (i < n) dst[i++] = 0;
    return dst;
}

char *strcat(char *dst, const char *src) {
    char *d = dst;
    while (*d) d++;
    while (src && *src) *d++ = *src++;
    *d = 0;
    return dst;
}

char *strncat(char *dst, const char *src, size_t n) {
    char *d = dst;
    while (*d) d++;
    for (size_t i = 0; i < n && src && src[i]; i++) *d++ = src[i];
    *d = 0;
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    while (s1 && s2 && *s1 && *s2) {
        if (*s1 != *s2) return (unsigned char)*s1 - (unsigned char)*s2;
        s1++; s2++;
    }
    if (!s1 || !*s1) return -(unsigned char)(s2 ? *s2 : 0);
    if (!s2 || !*s2) return (unsigned char)*s1;
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (!s1 || !s2 || !s1[i] || !s2[i]) break;
        if (s1[i] != s2[i]) return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}

char *strchr(const char *s, int c) {
    while (s && *s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    if (c == 0) return (char *)s;
    return NULL;
}

char *strrchr(const char *s, int c) {
    char *last = NULL;
    while (s && *s) {
        if (*s == (char)c) last = (char *)s;
        s++;
    }
    if (c == 0) return (char *)s;
    return last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!haystack || !needle || !*needle) return (char *)haystack;
    size_t nlen = strlen(needle);
    while (haystack && *haystack) {
        if (strncmp(haystack, needle, nlen) == 0) return (char *)haystack;
        haystack++;
    }
    return NULL;
}

char *strtok(char *str, const char *delim) {
    static char *strtok_state = NULL;
    if (str) strtok_state = str;
    if (!strtok_state || !*strtok_state) return NULL;
    
    while (*strtok_state && strchr(delim, *strtok_state)) strtok_state++;
    if (!*strtok_state) return NULL;
    
    char *start = strtok_state;
    while (*strtok_state && !strchr(delim, *strtok_state)) strtok_state++;
    if (*strtok_state) *strtok_state++ = 0;
    
    return start;
}

char *strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) strcpy(dup, s);
    return dup;
}

char *strndup(const char *s, size_t n) {
    if (!s) return NULL;
    size_t len = strnlen(s, n);
    char *dup = malloc(len + 1);
    if (dup) {
        strncpy(dup, s, len);
        dup[len] = 0;
    }
    return dup;
}

/* memory */
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    for (size_t i = 0; i < n; i++) p[i] = (uint8_t)c;
    return s;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
    }
    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *a = (const uint8_t *)s1;
    const uint8_t *b = (const uint8_t *)s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t *)s;
    for (size_t i = 0; i < n; i++) {
        if (p[i] == (uint8_t)c) return (void *)(p + i);
    }
    return NULL;
}

/* ctype */
int isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
int isdigit(int c) { return c >= '0' && c <= '9'; }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isspace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
int isupper(int c) { return c >= 'A' && c <= 'Z'; }
int islower(int c) { return c >= 'a' && c <= 'z'; }
int isprint(int c) { return c >= 32 && c <= 126; }
int isgraph(int c) { return c > 32 && c <= 126; }
int iscntrl(int c) { return c < 32 || c == 127; }
int ispunct(int c) { return isprint(c) && !isalnum(c) && !isspace(c); }
int isxdigit(int c) { return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
int isblank(int c) { return c == ' ' || c == '\t'; }
int toupper(int c) { return islower(c) ? c - 32 : c; }
int tolower(int c) { return isupper(c) ? c + 32 : c; }

/* stdlib */
int atoi(const char *nptr) {
    int n = 0, neg = 0;
    while (nptr && isspace(*nptr)) nptr++;
    if (*nptr == '-') { neg = 1; nptr++; }
    else if (*nptr == '+') nptr++;
    while (nptr && isdigit(*nptr)) n = n * 10 + (*nptr++ - '0');
    return neg ? -n : n;
}

long atol(const char *nptr) { return (long)atoi(nptr); }

long long atoll(const char *nptr) {
    long long n = 0;
    int neg = 0;
    while (nptr && isspace(*nptr)) nptr++;
    if (*nptr == '-') { neg = 1; nptr++; }
    else if (*nptr == '+') nptr++;
    while (nptr && isdigit(*nptr)) n = n * 10 + (*nptr++ - '0');
    return neg ? -n : n;
}

long strtol(const char *nptr, char **endptr, int base) {
    long n = 0;
    int neg = 0;
    while (nptr && isspace(*nptr)) nptr++;
    if (*nptr == '-') { neg = 1; nptr++; }
    else if (*nptr == '+') nptr++;
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') { base = 16; nptr += 2; }
            else base = 8;
        } else base = 10;
    }
    while (nptr && *nptr) {
        int digit = -1;
        if (isdigit(*nptr)) digit = *nptr - '0';
        else if (isalpha(*nptr)) digit = tolower(*nptr) - 'a' + 10;
        if (digit < 0 || digit >= base) break;
        n = n * base + digit;
        nptr++;
    }
    if (endptr) *endptr = (char *)nptr;
    return neg ? -n : n;
}

long long strtoll(const char *nptr, char **endptr, int base) {
    long long n = 0;
    int neg = 0;
    while (nptr && isspace(*nptr)) nptr++;
    if (*nptr == '-') { neg = 1; nptr++; }
    else if (*nptr == '+') nptr++;
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') { base = 16; nptr += 2; }
            else base = 8;
        } else base = 10;
    }
    while (nptr && *nptr) {
        int digit = -1;
        if (isdigit(*nptr)) digit = *nptr - '0';
        else if (isalpha(*nptr)) digit = tolower(*nptr) - 'a' + 10;
        if (digit < 0 || digit >= base) break;
        n = n * base + digit;
        nptr++;
    }
    if (endptr) *endptr = (char *)nptr;
    return neg ? -n : n;
}

unsigned long strtoul(const char *nptr, char **endptr, int base) {
    return (unsigned long)strtol(nptr, endptr, base);
}

unsigned long long strtoull(const char *nptr, char **endptr, int base) {
    return (unsigned long long)strtoll(nptr, endptr, base);
}

double strtod(const char *nptr, char **endptr) {
    double n = 0.0;
    int neg = 0;
    while (nptr && isspace(*nptr)) nptr++;
    if (*nptr == '-') { neg = 1; nptr++; }
    else if (*nptr == '+') nptr++;
    while (nptr && isdigit(*nptr)) n = n * 10.0 + (*nptr++ - '0');
    if (*nptr == '.') {
        nptr++;
        double frac = 0.1;
        while (nptr && isdigit(*nptr)) {
            n += frac * (*nptr++ - '0');
            frac *= 0.1;
        }
    }
    if (endptr) *endptr = (char *)nptr;
    return neg ? -n : n;
}

float strtof(const char *nptr, char **endptr) {
    return (float)strtod(nptr, endptr);
}

long double strtold(const char *nptr, char **endptr) {
    return (long double)strtod(nptr, endptr);
}

/* malloc - bump allocator, heap_base set by kernel.c heap_init() */
uint8_t *_heap_base = NULL;
uint64_t _heap_size = 0;
uint64_t _heap_used = 0;

void *malloc(size_t size) {
    if (!_heap_base || _heap_used + size > _heap_size) return NULL;
    void *ptr = _heap_base + _heap_used;
    _heap_used += (size + 15) & ~15;
    return ptr;
}

void free(void *ptr) { (void)ptr; }

void *calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = malloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    (void)ptr;
    return malloc(size);
}

void *aligned_alloc(size_t alignment, size_t size) {
    (void)alignment;
    return malloc(size);
}

/* math */
int abs(int j) { return j < 0 ? -j : j; }
long labs(long j) { return j < 0 ? -j : j; }
long long llabs(long long j) { return j < 0 ? -j : j; }

div_t div(int numer, int denom) {
    div_t d;
    d.quot = numer / denom;
    d.rem = numer % denom;
    return d;
}

ldiv_t ldiv(long numer, long denom) {
    ldiv_t d;
    d.quot = numer / denom;
    d.rem = numer % denom;
    return d;
}

lldiv_t lldiv(long long numer, long long denom) {
    lldiv_t d;
    d.quot = numer / denom;
    d.rem = numer % denom;
    return d;
}

static uint32_t rand_seed = 1;
int rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}

void srand(unsigned int seed) { rand_seed = seed; }

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    if (nmemb <= 1) return;
    uint8_t *arr = (uint8_t *)base;
    uint8_t tmp[256];
    for (size_t i = 1; i < nmemb; i++) {
        for (size_t j = i; j > 0; j--) {
            if (compar(&arr[(j-1)*size], &arr[j*size]) <= 0) break;
            memcpy(tmp, &arr[(j-1)*size], size);
            memcpy(&arr[(j-1)*size], &arr[j*size], size);
            memcpy(&arr[j*size], tmp, size);
        }
    }
}

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    size_t left = 0, right = nmemb;
    const uint8_t *arr = (const uint8_t *)base;
    while (left < right) {
        size_t mid = (left + right) / 2;
        int cmp = compar(key, &arr[mid * size]);
        if (cmp == 0) return (void *)&arr[mid * size];
        if (cmp < 0) right = mid;
        else left = mid + 1;
    }
    return NULL;
}

/* math functions */
double sqrt(double x) {
    if (x < 0) return 0;
    if (x == 0) return 0;
    double guess = x / 2;
    for (int i = 0; i < 10; i++) guess = (guess + x / guess) / 2;
    return guess;
}

double pow(double x, double y) {
    if (y == 0) return 1;
    if (y == 1) return x;
    double result = 1;
    for (int i = 0; i < (int)y; i++) result *= x;
    return result;
}

double fabs(double x) { return x < 0 ? -x : x; }
double floor(double x) { return (double)(long long)x; }
double ceil(double x) { long long n = (long long)x; return (x > n) ? n + 1 : n; }
double round(double x) { return floor(x + 0.5); }
double trunc(double x) { return (double)(long long)x; }

double cbrt(double x) { return pow(x, 1.0/3.0); }
double hypot(double x, double y) { return sqrt(x*x + y*y); }
double fmod(double x, double y) { return x - (long long)(x/y) * y; }

double sin(double x) { (void)x; return 0; }
double cos(double x) { (void)x; return 1; }
double tan(double x) { (void)x; return 0; }
double asin(double x) { (void)x; return 0; }
double acos(double x) { (void)x; return 0; }
double atan(double x) { (void)x; return 0; }
double atan2(double y, double x) { (void)y; (void)x; return 0; }

double sinh(double x) { (void)x; return 0; }
double cosh(double x) { (void)x; return 1; }
double tanh(double x) { (void)x; return 0; }
double asinh(double x) { (void)x; return 0; }
double acosh(double x) { (void)x; return 0; }
double atanh(double x) { (void)x; return 0; }

double exp(double x) { (void)x; return 1; }
double exp2(double x) { (void)x; return 1; }
double exp10(double x) { (void)x; return 1; }
double expm1(double x) { (void)x; return 0; }
double log(double x) { (void)x; return 0; }
double log10(double x) { (void)x; return 0; }
double log2(double x) { (void)x; return 0; }
double log1p(double x) { (void)x; return 0; }

double ldexp(double x, int exp) { (void)exp; return x; }
double frexp(double x, int *exp) { if (exp) *exp = 0; return x; }
double modf(double x, double *ipart) { if (ipart) *ipart = (double)(long long)x; return x - (double)(long long)x; }

double copysign(double x, double y) { return (y < 0) ? -fabs(x) : fabs(x); }
double fdim(double x, double y) { return (x > y) ? x - y : 0; }
double fmax(double x, double y) { return (x > y) ? x : y; }
double fmin(double x, double y) { return (x < y) ? x : y; }

int isnan(double x) { return x != x; }
int isinf(double x) { return x > 1e308 || x < -1e308; }
int isfinite(double x) { return !isnan(x) && !isinf(x); }
int isnormal(double x) { return isfinite(x) && x != 0; }

/* time */
time_t time(time_t *timer) {
    time_t t = 0;
    if (timer) *timer = t;
    return t;
}

clock_t clock(void) { return 0; }
double difftime(time_t time1, time_t time0) { return (double)(time1 - time0); }

/* assert */
void assert(int expression) {
    if (!expression) for (;;) __asm__("hlt");
}

/* exit */
_Noreturn void exit(int status) {
    (void)status;
    for (;;) __asm__("hlt");
}

_Noreturn void abort(void) {
    for (;;) __asm__("hlt");
}
