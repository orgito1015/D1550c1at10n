#ifndef NETWORK_RTL8139_H
#define NETWORK_RTL8139_H

#include <stdint.h>

int rtl8139_init(void);
int rtl8139_send_packet(const void *data, uint32_t len);
int rtl8139_receive_packet(void *data, uint32_t max_len);

#endif
