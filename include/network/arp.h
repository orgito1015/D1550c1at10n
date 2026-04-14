#ifndef NETWORK_ARP_H
#define NETWORK_ARP_H

#include <stddef.h>
#include <stdint.h>

void arp_set_local_identity(uint32_t local_ip, const uint8_t local_mac[6]);
int arp_send_request(uint32_t ip, uint8_t *out_frame, size_t *out_len);
int arp_handle_packet(const uint8_t *frame, size_t len, uint8_t *out_reply, size_t *out_reply_len);
int arp_resolve(uint32_t ip, uint8_t out_mac[6]);

#endif
