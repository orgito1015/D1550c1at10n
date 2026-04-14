#include "tests/test_common.h"

#include "display/vbe.h"
#include "input/mouse.h"
#include "mocks/mock_hw.h"

int test_framebuffer_pixel_draw(void) {
    mock_hw_reset();
    ASSERT_TRUE(vbe_init(320, 200) == 0);
    ASSERT_TRUE(vbe_draw_pixel(10, 12, 0xFFAABBCCU) == 0);
    ASSERT_TRUE(vbe_get_pixel(10, 12) == 0xFFAABBCCU);
    return 0;
}

int test_mouse_packet_movement(void) {
    mouse_state_t state;
    uint8_t packet[3] = {0x01, 0x05, 0x03};

    mock_hw_reset();
    mouse_init();
    ASSERT_TRUE(mock_mouse_push_packet(packet) == 0);
    ASSERT_TRUE(mouse_poll(&state) == 0);
    ASSERT_TRUE(state.left == 1);
    ASSERT_TRUE(state.x == 5);
    ASSERT_TRUE(state.y == -3);
    return 0;
}
