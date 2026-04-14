#include "display/font.h"
#include "display/vbe.h"

void font_draw_char(char c, int x, int y, uint32_t color) {
    int row;
    int col;
    int fill = (c % 2) == 0;

    for (row = 0; row < 8; ++row) {
        for (col = 0; col < 8; ++col) {
            if (fill || row == 0 || row == 7 || col == 0 || col == 7) {
                (void)vbe_draw_pixel((uint32_t)(x + col), (uint32_t)(y + row), color);
            }
        }
    }
}
