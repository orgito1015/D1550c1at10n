#include "network/rtl8139.h"
#include "mocks/mock_hw.h"

#include <stdint.h>

int rtl8139_init(void) { return 0; }

int rtl8139_send_packet(const void *data, uint32_t len) {
    return mock_nic_push_tx_packet((const uint8_t *)data, len);
}

int rtl8139_receive_packet(void *data, uint32_t max_len) {
    uint32_t out_len = 0;
    if (mock_nic_pop_rx_packet((uint8_t *)data, max_len, &out_len) != 0) {
        return -1;
    }
    return (int)out_len;
}
