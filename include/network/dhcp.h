#ifndef NETWORK_DHCP_H
#define NETWORK_DHCP_H

#include <stdint.h>

int dhcp_discover(void);
int dhcp_request(uint32_t offered_ip);
uint32_t dhcp_get_lease_ip(void);
int dhcp_run_mock_flow(uint32_t offered_ip, uint32_t *leased_ip);

#endif
