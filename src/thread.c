#include "thread.h"

#define MAX_THREADS 8

typedef struct {
    thread_func_t func;
    uint8_t active;
} thread_t;

static thread_t threads[MAX_THREADS];
static int thread_count = 0;
static int current_thread = 0;

void thread_init(void) {
    thread_count = 0;
    current_thread = 0;
}

void thread_create(thread_func_t func) {
    if (thread_count >= MAX_THREADS) return;
    threads[thread_count].func = func;
    threads[thread_count].active = 1;
    thread_count++;
}

void thread_yield(void) {
    if (thread_count == 0) return;
    current_thread = (current_thread + 1) % thread_count;
    if (threads[current_thread].active && threads[current_thread].func) {
        threads[current_thread].func();
    }
}
