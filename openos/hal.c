/*
 * openos/hal.c
 *
 * OpenOS HAL implementation.
 *
 * This file implements the driver-facing mock_hw.h interface (all mock_*
 * symbols) by delegating every call to the corresponding openos_* function
 * declared in include/openos/hal.h.  When building for OpenOS, compile this
 * file instead of mocks/mock_hw.c; driver and framework source files require
 * no modifications.
 *
 * Functions that are meaningful only in the standalone test harness
 * (mock_hw_reset, mock_pci_set_device_count, mock_nic_push_rx_packet,
 * mock_nic_pop_tx_packet, mock_mouse_push_packet, mock_uart_feed_rx,
 * mock_uart_take_tx, mock_irq_trigger) become no-ops or return -1 to signal
 * that the operation is not supported in a production environment.
 */

#include "mocks/mock_hw.h"
#include "openos/hal.h"

/* ------------------------------------------------------------------ */
/* Reset – no-op in production; the kernel owns hardware state.       */
/* ------------------------------------------------------------------ */
void mock_hw_reset(void) {}

/* ------------------------------------------------------------------ */
/* Port I/O                                                            */
/* ------------------------------------------------------------------ */
uint8_t  mock_inb(uint16_t port)              { return openos_inb(port); }
void     mock_outb(uint16_t port, uint8_t v)  { openos_outb(port, v); }
uint16_t mock_inw(uint16_t port)              { return openos_inw(port); }
void     mock_outw(uint16_t port, uint16_t v) { openos_outw(port, v); }

/* ------------------------------------------------------------------ */
/* PCI                                                                 */
/* ------------------------------------------------------------------ */
void     mock_pci_set_device_count(uint32_t n) { (void)n; }
uint32_t mock_pci_scan(void)                   { return openos_pci_device_count(); }

/* ------------------------------------------------------------------ */
/* IRQ                                                                 */
/* ------------------------------------------------------------------ */
void mock_irq_register(int irq, mock_irq_handler_t handler)
{
    openos_irq_register(irq, handler);
}

/* irq_trigger is a test-only helper; hardware IRQs fire asynchronously. */
int mock_irq_trigger(int irq) { (void)irq; return -1; }

/* ------------------------------------------------------------------ */
/* Framebuffer                                                         */
/* ------------------------------------------------------------------ */
static uint32_t s_fb_width;
static uint32_t s_fb_height;

int mock_framebuffer_init(uint32_t width, uint32_t height)
{
    if (openos_framebuffer_request(width, height) != 0) {
        return -1;
    }
    s_fb_width  = openos_framebuffer_width();
    s_fb_height = openos_framebuffer_height();
    return 0;
}

uint32_t *mock_framebuffer_data(void)   { return openos_framebuffer_addr(); }
uint32_t  mock_framebuffer_width(void)  { return s_fb_width; }
uint32_t  mock_framebuffer_height(void) { return s_fb_height; }

/* ------------------------------------------------------------------ */
/* Storage: ATA                                                        */
/* ------------------------------------------------------------------ */
int mock_ata_read_sector(uint32_t lba, uint8_t *buf)
{
    return openos_ata_read(lba, buf);
}

int mock_ata_write_sector(uint32_t lba, const uint8_t *buf)
{
    return openos_ata_write(lba, buf);
}

/* ------------------------------------------------------------------ */
/* Storage: RAM disk                                                   */
/* ------------------------------------------------------------------ */
int mock_ramdisk_read_sector(uint32_t lba, uint8_t *buf)
{
    return openos_ramdisk_read(lba, buf);
}

int mock_ramdisk_write_sector(uint32_t lba, const uint8_t *buf)
{
    return openos_ramdisk_write(lba, buf);
}

/* ------------------------------------------------------------------ */
/* Network: NIC                                                        */
/* ------------------------------------------------------------------ */
int mock_nic_push_tx_packet(const uint8_t *data, uint32_t len)
{
    return openos_nic_tx(data, len);
}

int mock_nic_pop_rx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len)
{
    return openos_nic_rx(out, max_len, out_len);
}

/* The inject/drain helpers are test-only; not reachable in production. */
int mock_nic_push_rx_packet(const uint8_t *data, uint32_t len)
{
    (void)data; (void)len; return -1;
}

int mock_nic_pop_tx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len)
{
    (void)out; (void)max_len; (void)out_len; return -1;
}

/* ------------------------------------------------------------------ */
/* Input: PS/2 mouse                                                   */
/* ------------------------------------------------------------------ */
int mock_mouse_pop_packet(uint8_t packet[3])
{
    return openos_mouse_read_packet(packet);
}

/* Test-only inject; not reachable in production. */
int mock_mouse_push_packet(const uint8_t packet[3])
{
    (void)packet; return -1;
}

/* ------------------------------------------------------------------ */
/* Input: UART                                                         */
/* ------------------------------------------------------------------ */
int mock_uart_read_byte(uint8_t *out)  { return openos_serial_read_byte(out); }
int mock_uart_write_byte(uint8_t byte) { return openos_serial_write_byte(byte); }

/* Test-only feed/drain; no-ops in production. */
void   mock_uart_feed_rx(const uint8_t *data, size_t len) { (void)data; (void)len; }
size_t mock_uart_take_tx(uint8_t *out, size_t max_len)    { (void)out; (void)max_len; return 0; }
