#include "input/mouse.h"
#include "mocks/mock_hw.h"

static mouse_state_t g_state;

void mouse_init(void) {
    g_state.x = 0;
    g_state.y = 0;
    g_state.left = 0;
    g_state.right = 0;
    g_state.middle = 0;
}

static int decode_signed(uint8_t v) {
    return (v & 0x80U) ? (int)v - 256 : (int)v;
}

int mouse_poll(mouse_state_t *state) {
    uint8_t packet[3];
    if (state == 0 || mock_mouse_pop_packet(packet) != 0) {
        return -1;
    }
    g_state.left = (packet[0] & 0x1U) ? 1 : 0;
    g_state.right = (packet[0] & 0x2U) ? 1 : 0;
    g_state.middle = (packet[0] & 0x4U) ? 1 : 0;
    g_state.x += decode_signed(packet[1]);
    g_state.y -= decode_signed(packet[2]);
    *state = g_state;
    return 0;
}
