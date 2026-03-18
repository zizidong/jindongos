#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <stdint.h>

typedef uint32_t pthread_t;
typedef struct {
    uint32_t dummy;
} pthread_attr_t;

typedef struct {
    uint32_t dummy;
} pthread_mutex_t;

typedef struct {
    uint32_t dummy;
} pthread_cond_t;

#define PTHREAD_MUTEX_INITIALIZER {0}
#define PTHREAD_COND_INITIALIZER {0}

typedef void *(*pthread_start_routine_t)(void *);

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   pthread_start_routine_t start_routine, void *arg);
int pthread_join(pthread_t thread, void **retval);
void pthread_exit(void *retval);

int pthread_mutex_init(pthread_mutex_t *mutex, const void *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

int pthread_cond_init(pthread_cond_t *cond, const void *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);

void pthread_schedule(void);

#endif
