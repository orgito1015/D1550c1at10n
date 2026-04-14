#include "tests/test_common.h"

#include "mocks/mock_hw.h"
#include "network/arp.h"
#include "network/dhcp.h"
#include "network/rtl8139.h"

#include <stdint.h>
#include <string.h>

int test_nic_send_receive(void) {
    uint8_t packet[64];
    uint8_t out[64];
    uint8_t tx[64];
    uint32_t tx_len = 0;
    int rx_len;

    mock_hw_reset();
    memset(packet, 0x5A, sizeof(packet));

    ASSERT_TRUE(rtl8139_init() == 0);
    ASSERT_TRUE(mock_nic_push_rx_packet(packet, sizeof(packet)) == 0);
    rx_len = rtl8139_receive_packet(out, sizeof(out));
    ASSERT_TRUE(rx_len == 64);
    ASSERT_TRUE(memcmp(packet, out, sizeof(packet)) == 0);

    ASSERT_TRUE(rtl8139_send_packet(packet, sizeof(packet)) == 0);
    ASSERT_TRUE(mock_nic_pop_tx_packet(tx, sizeof(tx), &tx_len) == 0);
    ASSERT_TRUE(tx_len == 64);
    ASSERT_TRUE(memcmp(packet, tx, sizeof(packet)) == 0);
    return 0;
}

int test_arp_resolution(void) {
    uint8_t request[42];
    uint8_t reply[42];
    size_t req_len = 0;
    size_t reply_len = 0;
    uint8_t mac[6];
    uint8_t peer_mac[6] = {0x10, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint32_t peer_ip = 0xC0A80114U;
    int handled;

    arp_set_local_identity(0xC0A8010AU, (uint8_t[]){0x02, 0x00, 0x00, 0x00, 0x00, 0x01});
    ASSERT_TRUE(arp_send_request(peer_ip, request, &req_len) == 0);
    ASSERT_TRUE(req_len == 42);

    memcpy(request + 22, peer_mac, 6);
    memcpy(request + 28, &peer_ip, 4);
    {
        uint32_t local_ip = 0xC0A8010AU;
        memcpy(request + 38, &local_ip, 4);
    }
    handled = arp_handle_packet(request, sizeof(request), reply, &reply_len);
    ASSERT_TRUE(handled == 1);
    ASSERT_TRUE(reply_len == 42);
    ASSERT_TRUE(arp_resolve(peer_ip, mac) == 0);
    ASSERT_TRUE(memcmp(mac, peer_mac, 6) == 0);
    return 0;
}

int test_dhcp_mock_flow(void) {
    uint32_t lease = 0;
    ASSERT_TRUE(dhcp_run_mock_flow(0xC0A80164U, &lease) == 0);
    ASSERT_TRUE(lease == 0xC0A80164U);
    return 0;
}
