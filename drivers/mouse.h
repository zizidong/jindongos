#ifndef _MOUSE_H
#define _MOUSE_H

#include <stdint.h>

extern int32_t mouse_x;
extern int32_t mouse_y;
extern uint8_t mouse_buttons;
extern uint8_t mouse_prev_buttons;

void mouse_init(void);
void mouse_set_screen(int32_t w, int32_t h);
void mouse_poll(void);   /* PS/2 포트 폴링 - 매 프레임 호출 */

#endif
