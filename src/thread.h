#ifndef _THREAD_H
#define _THREAD_H

#include <stdint.h>

typedef void (*thread_func_t)(void);

void thread_init(void);
void thread_create(thread_func_t func);
void thread_yield(void);  /* context switch */

#endif
