#include <stdint.h>
#include <stddef.h>

typedef struct {
    void *front_buffer;
    void *back_buffer;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t buffer_size;
} buffer_t;

static buffer_t buf = {0};
static uint8_t back_buffer_mem[2097152];

void buffer_init(void *fb, uint64_t width, uint64_t height, uint64_t pitch) {
    buf.front_buffer = fb;
    buf.back_buffer = back_buffer_mem;
    buf.width = width;
    buf.height = height;
    buf.pitch = pitch;
    buf.buffer_size = height * pitch;
}

void buffer_clear(uint32_t color) {
    uint32_t *b = (uint32_t *)buf.back_buffer;
    uint64_t count = buf.height * buf.pitch / 4;
    for (uint64_t i = 0; i < count; i++) {
        b[i] = color;
    }
}

void buffer_draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= buf.width || y >= buf.height) return;
    uint32_t *b = (uint32_t *)buf.back_buffer;
    b[y * (buf.pitch / 4) + x] = color;
}

void buffer_swap(void) {
    uint8_t *src = (uint8_t *)buf.back_buffer;
    uint8_t *dst = (uint8_t *)buf.front_buffer;
    
    for (uint64_t i = 0; i < buf.buffer_size; i++) {
        dst[i] = src[i];
    }
}
