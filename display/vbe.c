#include "display/vbe.h"
#include "mocks/mock_hw.h"

#include <stddef.h>

static uint32_t *g_fb;
static uint32_t g_width;
static uint32_t g_height;

int vbe_init(uint32_t width, uint32_t height) {
    if (mock_framebuffer_init(width, height) != 0) {
        return -1;
    }
    g_fb = mock_framebuffer_data();
    g_width = mock_framebuffer_width();
    g_height = mock_framebuffer_height();
    return 0;
}

int vbe_draw_pixel(uint32_t x, uint32_t y, uint32_t argb) {
    if (g_fb == NULL || x >= g_width || y >= g_height) {
        return -1;
    }
    g_fb[y * g_width + x] = argb;
    return 0;
}

uint32_t vbe_get_pixel(uint32_t x, uint32_t y) {
    if (g_fb == NULL || x >= g_width || y >= g_height) {
        return 0;
    }
    return g_fb[y * g_width + x];
}
