#ifndef DISPLAY_VBE_H
#define DISPLAY_VBE_H

#include <stdint.h>

int vbe_init(uint32_t width, uint32_t height);
int vbe_draw_pixel(uint32_t x, uint32_t y, uint32_t argb);
uint32_t vbe_get_pixel(uint32_t x, uint32_t y);

#endif
