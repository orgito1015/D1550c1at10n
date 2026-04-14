#include "display/cursor.h"

static int g_cursor_x;
static int g_cursor_y;

void cursor_move(int x, int y) {
    g_cursor_x = x;
    g_cursor_y = y;
}

int cursor_x(void) { return g_cursor_x; }
int cursor_y(void) { return g_cursor_y; }
