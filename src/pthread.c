#include "pthread.h"
#include <stddef.h>

#define MAX_THREADS 16

typedef struct {
    pthread_start_routine_t func;
    void *arg;
    void *retval;
    uint8_t active;
    uint8_t joined;
} thread_info_t;

static thread_info_t threads[MAX_THREADS];
static int thread_count = 0;
static int current_thread = -1;
static pthread_t next_tid = 1;

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   pthread_start_routine_t start_routine, void *arg) {
    (void)attr;
    if (thread_count >= MAX_THREADS) return -1;
    
    int idx = thread_count++;
    threads[idx].func = start_routine;
    threads[idx].arg = arg;
    threads[idx].retval = NULL;
    threads[idx].active = 1;
    threads[idx].joined = 0;
    
    *thread = next_tid++;
    return 0;
}

int pthread_join(pthread_t thread, void **retval) {
    (void)thread;  /* unused */
    for (int i = 0; i < thread_count; i++) {
        if (threads[i].active) {
            threads[i].joined = 1;
            if (retval) *retval = threads[i].retval;
            return 0;
        }
    }
    return -1;
}

void pthread_exit(void *retval) {
    if (current_thread >= 0 && current_thread < thread_count) {
        threads[current_thread].retval = retval;
        threads[current_thread].active = 0;
    }
}

int pthread_mutex_init(pthread_mutex_t *mutex, const void *attr) {
    (void)mutex; (void)attr;
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    (void)mutex;
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    (void)mutex;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    (void)mutex;
    return 0;
}

int pthread_cond_init(pthread_cond_t *cond, const void *attr) {
    (void)cond; (void)attr;
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    (void)cond; (void)mutex;
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond) {
    (void)cond;
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
    (void)cond;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
    (void)cond;
    return 0;
}

/* 스레드 스케줄러 - 메인 루프에서 호출 */
void pthread_schedule(void) {
    if (thread_count == 0) return;
    
    current_thread = (current_thread + 1) % thread_count;
    
    if (threads[current_thread].active && threads[current_thread].func) {
        void *ret = threads[current_thread].func(threads[current_thread].arg);
        threads[current_thread].retval = ret;
    }
}
