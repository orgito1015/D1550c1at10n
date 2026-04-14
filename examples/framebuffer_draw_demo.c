#include "display/font.h"
#include "display/vbe.h"

#include <stdio.h>

int main(void) {
    (void)vbe_init(320, 200);
    (void)vbe_draw_pixel(1, 1, 0xFFFFFFFFU);
    font_draw_char('A', 8, 8, 0xFF00FF00U);
    printf("Framebuffer demo rendered pixel=%08X\n", vbe_get_pixel(1, 1));
    return 0;
}
