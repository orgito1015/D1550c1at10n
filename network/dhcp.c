#include "network/dhcp.h"

enum dhcp_state {
    DHCP_IDLE,
    DHCP_DISCOVER_SENT,
    DHCP_REQUEST_SENT,
    DHCP_BOUND
};

static enum dhcp_state g_state;
static uint32_t g_offer_ip;
static uint32_t g_lease_ip;

int dhcp_discover(void) {
    g_state = DHCP_DISCOVER_SENT;
    g_offer_ip = 0;
    return 0;
}

int dhcp_request(uint32_t offered_ip) {
    if (g_state != DHCP_DISCOVER_SENT || offered_ip == 0) {
        return -1;
    }
    g_offer_ip = offered_ip;
    g_state = DHCP_REQUEST_SENT;
    g_lease_ip = offered_ip;
    g_state = DHCP_BOUND;
    return 0;
}

uint32_t dhcp_get_lease_ip(void) {
    return g_state == DHCP_BOUND ? g_lease_ip : 0;
}

int dhcp_run_mock_flow(uint32_t offered_ip, uint32_t *leased_ip) {
    if (dhcp_discover() != 0 || dhcp_request(offered_ip) != 0) {
        return -1;
    }
    if (leased_ip != 0) {
        *leased_ip = dhcp_get_lease_ip();
    }
    return 0;
}
