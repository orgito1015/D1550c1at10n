#include "mocks/mock_hw.h"
#include "network/rtl8139.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    unsigned char packet[32];
    unsigned char out[32];
    int rx_len;

    mock_hw_reset();
    memset(packet, 0xCD, sizeof(packet));
    (void)rtl8139_init();
    (void)mock_nic_push_rx_packet(packet, sizeof(packet));
    rx_len = rtl8139_receive_packet(out, sizeof(out));
    printf("Received packet length: %d\n", rx_len);
    return 0;
}
