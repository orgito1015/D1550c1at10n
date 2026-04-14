#ifndef MOCK_HW_H
#define MOCK_HW_H

#include <stddef.h>
#include <stdint.h>

#define MOCK_SECTOR_SIZE 512U
#define MOCK_ATA_SECTORS 4096U
#define MOCK_RAMDISK_SECTORS 2048U
#define MOCK_MAX_PACKET_SIZE 1536U
#define MOCK_PACKET_RING_SIZE 8U

typedef void (*mock_irq_handler_t)(void);

void mock_hw_reset(void);

uint8_t mock_inb(uint16_t port);
void mock_outb(uint16_t port, uint8_t value);
uint16_t mock_inw(uint16_t port);
void mock_outw(uint16_t port, uint16_t value);

void mock_pci_set_device_count(uint32_t device_count);
uint32_t mock_pci_scan(void);

void mock_irq_register(int irq, mock_irq_handler_t handler);
int mock_irq_trigger(int irq);

int mock_framebuffer_init(uint32_t width, uint32_t height);
uint32_t *mock_framebuffer_data(void);
uint32_t mock_framebuffer_width(void);
uint32_t mock_framebuffer_height(void);

int mock_ata_read_sector(uint32_t lba, uint8_t *buffer);
int mock_ata_write_sector(uint32_t lba, const uint8_t *buffer);
int mock_ramdisk_read_sector(uint32_t lba, uint8_t *buffer);
int mock_ramdisk_write_sector(uint32_t lba, const uint8_t *buffer);

int mock_nic_push_rx_packet(const uint8_t *data, uint32_t len);
int mock_nic_pop_rx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len);
int mock_nic_push_tx_packet(const uint8_t *data, uint32_t len);
int mock_nic_pop_tx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len);

int mock_mouse_push_packet(const uint8_t packet[3]);
int mock_mouse_pop_packet(uint8_t packet[3]);

void mock_uart_feed_rx(const uint8_t *data, size_t len);
int mock_uart_read_byte(uint8_t *out_byte);
int mock_uart_write_byte(uint8_t byte);
size_t mock_uart_take_tx(uint8_t *out, size_t max_len);

#endif
