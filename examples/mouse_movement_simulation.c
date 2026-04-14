#include "input/mouse.h"
#include "mocks/mock_hw.h"

#include <stdio.h>

int main(void) {
    mouse_state_t state;
    unsigned char packet[3] = {0x01, 0x04, 0x02};

    mock_hw_reset();
    mouse_init();
    (void)mock_mouse_push_packet(packet);
    if (mouse_poll(&state) == 0) {
        printf("Mouse: x=%d y=%d left=%d\n", state.x, state.y, state.left);
    }
    return 0;
}
