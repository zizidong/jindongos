#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>

typedef struct {
    uint8_t scancode;
    char    ascii;
    uint8_t pressed;
} keyboard_event_t;

extern keyboard_event_t last_key;

void keyboard_init(void);
void keyboard_poll(void);   /* PS/2 폴링 - 매 프레임 호출 */

#endif
