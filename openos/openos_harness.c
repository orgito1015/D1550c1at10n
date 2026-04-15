/*
 * openos/openos_harness.c
 *
 * In-process implementation of every openos_* function declared in
 * include/openos/hal.h.  Backed by the same deterministic in-memory
 * arrays as mocks/mock_hw.c so that the full driver → HAL → harness
 * call chain can be exercised without a real kernel.
 *
 * This file is only compiled into the openos-test binary.
 */

#include "openos/openos_harness.h"
#include "openos/hal.h"

#include <string.h>

/* ------------------------------------------------------------------ */
/* Sizes (mirror mock_hw.h constants without including that header)   */
/* ------------------------------------------------------------------ */
#define H_SECTOR_SIZE      512U
#define H_ATA_SECTORS      4096U
#define H_RAMDISK_SECTORS  2048U
#define H_MAX_PACKET_SIZE  1536U
#define H_PACKET_RING_SIZE 8U
#define H_MOUSE_QUEUE_SIZE 32U
#define H_UART_BUF_SIZE    256U
#define H_MAX_IRQS         16
#define H_FB_MAX_PIXELS    (1024U * 768U)

/* ------------------------------------------------------------------ */
/* Storage state                                                       */
/* ------------------------------------------------------------------ */
static uint8_t h_ata[H_ATA_SECTORS][H_SECTOR_SIZE];
static uint8_t h_ramdisk[H_RAMDISK_SECTORS][H_SECTOR_SIZE];

/* ------------------------------------------------------------------ */
/* Framebuffer state                                                   */
/* ------------------------------------------------------------------ */
static uint32_t h_fb[H_FB_MAX_PIXELS];
static uint32_t h_fb_width;
static uint32_t h_fb_height;

/* ------------------------------------------------------------------ */
/* Network state                                                       */
/* ------------------------------------------------------------------ */
typedef struct h_pkt {
    uint8_t  data[H_MAX_PACKET_SIZE];
    uint32_t len;
    int      used;
} h_pkt_t;

static h_pkt_t   h_rx_ring[H_PACKET_RING_SIZE];
static h_pkt_t   h_tx_ring[H_PACKET_RING_SIZE];
static unsigned  h_rx_head;
static unsigned  h_rx_tail;
static unsigned  h_tx_head;
static unsigned  h_tx_tail;

/* ------------------------------------------------------------------ */
/* IRQ state                                                           */
/* ------------------------------------------------------------------ */
static void (*h_irq_handlers[H_MAX_IRQS])(void);

/* ------------------------------------------------------------------ */
/* Mouse state                                                         */
/* ------------------------------------------------------------------ */
static uint8_t  h_mouse_queue[H_MOUSE_QUEUE_SIZE][3];
static unsigned h_mouse_head;
static unsigned h_mouse_tail;

/* ------------------------------------------------------------------ */
/* UART state                                                          */
/* ------------------------------------------------------------------ */
static uint8_t h_uart_rx[H_UART_BUF_SIZE];
static size_t  h_uart_rx_len;
static size_t  h_uart_rx_pos;
static uint8_t h_uart_tx[H_UART_BUF_SIZE];
static size_t  h_uart_tx_len;

/* ------------------------------------------------------------------ */
/* PCI state                                                           */
/* ------------------------------------------------------------------ */
static uint32_t h_pci_devices;

/* ================================================================== */
/* Public reset                                                        */
/* ================================================================== */

void openos_harness_reset(void)
{
    memset(h_ata,          0, sizeof(h_ata));
    memset(h_ramdisk,      0, sizeof(h_ramdisk));
    memset(h_fb,           0, sizeof(h_fb));
    memset(h_rx_ring,      0, sizeof(h_rx_ring));
    memset(h_tx_ring,      0, sizeof(h_tx_ring));
    memset(h_irq_handlers, 0, sizeof(h_irq_handlers));
    memset(h_mouse_queue,  0, sizeof(h_mouse_queue));
    memset(h_uart_rx,      0, sizeof(h_uart_rx));
    memset(h_uart_tx,      0, sizeof(h_uart_tx));

    h_fb_width  = 640U;
    h_fb_height = 480U;
    h_pci_devices = 1U;
    h_rx_head = h_rx_tail = 0U;
    h_tx_head = h_tx_tail = 0U;
    h_mouse_head = h_mouse_tail = 0U;
    h_uart_rx_len = h_uart_rx_pos = 0U;
    h_uart_tx_len = 0U;
}

/* ================================================================== */
/* openos_* implementations (called by openos/hal.c)                  */
/* ================================================================== */

/* Port I/O — backed by two flat arrays indexed by port number. */
static uint8_t  h_ports8[65536];
static uint16_t h_ports16[65536];

uint8_t  openos_inb(uint16_t port)              { return h_ports8[port]; }
void     openos_outb(uint16_t port, uint8_t v)  { h_ports8[port] = v; }
uint16_t openos_inw(uint16_t port)              { return h_ports16[port]; }
void     openos_outw(uint16_t port, uint16_t v) { h_ports16[port] = v; }

/* PCI */
uint32_t openos_pci_device_count(void) { return h_pci_devices; }

/* IRQ */
void openos_irq_register(int irq, void (*handler)(void))
{
    if (irq >= 0 && irq < H_MAX_IRQS) {
        h_irq_handlers[irq] = handler;
    }
}

/* Framebuffer */
int openos_framebuffer_request(uint32_t width, uint32_t height)
{
    if (width == 0U || height == 0U || width > 1024U || height > 768U) {
        return -1;
    }
    h_fb_width  = width;
    h_fb_height = height;
    memset(h_fb, 0, sizeof(h_fb));
    return 0;
}

uint32_t *openos_framebuffer_addr(void)   { return h_fb; }
uint32_t  openos_framebuffer_width(void)  { return h_fb_width; }
uint32_t  openos_framebuffer_height(void) { return h_fb_height; }

/* ATA */
int openos_ata_read(uint32_t lba, uint8_t *buf)
{
    if (lba >= H_ATA_SECTORS || buf == NULL) { return -1; }
    memcpy(buf, h_ata[lba], H_SECTOR_SIZE);
    return 0;
}

int openos_ata_write(uint32_t lba, const uint8_t *buf)
{
    if (lba >= H_ATA_SECTORS || buf == NULL) { return -1; }
    memcpy(h_ata[lba], buf, H_SECTOR_SIZE);
    return 0;
}

/* RAM disk */
int openos_ramdisk_read(uint32_t lba, uint8_t *buf)
{
    if (lba >= H_RAMDISK_SECTORS || buf == NULL) { return -1; }
    memcpy(buf, h_ramdisk[lba], H_SECTOR_SIZE);
    return 0;
}

int openos_ramdisk_write(uint32_t lba, const uint8_t *buf)
{
    if (lba >= H_RAMDISK_SECTORS || buf == NULL) { return -1; }
    memcpy(h_ramdisk[lba], buf, H_SECTOR_SIZE);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Internal ring helpers (shared by NIC and harness test helpers)     */
/* ------------------------------------------------------------------ */

static int ring_push(h_pkt_t *ring, unsigned *head, unsigned *tail,
                     const uint8_t *data, uint32_t len)
{
    unsigned next = (*head + 1U) % H_PACKET_RING_SIZE;
    if (next == *tail || data == NULL || len == 0U || len > H_MAX_PACKET_SIZE) {
        return -1;
    }
    memcpy(ring[*head].data, data, len);
    ring[*head].len  = len;
    ring[*head].used = 1;
    *head = next;
    return 0;
}

static int ring_pop(h_pkt_t *ring, unsigned *head, unsigned *tail,
                    uint8_t *out, uint32_t max_len, uint32_t *out_len)
{
    if (*tail == *head || out == NULL || out_len == NULL) { return -1; }
    if (!ring[*tail].used || ring[*tail].len > max_len)   { return -1; }
    memcpy(out, ring[*tail].data, ring[*tail].len);
    *out_len = ring[*tail].len;
    ring[*tail].used = 0;
    *tail = (*tail + 1U) % H_PACKET_RING_SIZE;
    return 0;
}

/* NIC */
int openos_nic_tx(const uint8_t *data, uint32_t len)
{
    return ring_push(h_tx_ring, &h_tx_head, &h_tx_tail, data, len);
}

int openos_nic_rx(uint8_t *out, uint32_t max_len, uint32_t *out_len)
{
    return ring_pop(h_rx_ring, &h_rx_head, &h_rx_tail, out, max_len, out_len);
}

/* Mouse */
int openos_mouse_read_packet(uint8_t packet[3])
{
    if (packet == NULL || h_mouse_head == h_mouse_tail) { return -1; }
    memcpy(packet, h_mouse_queue[h_mouse_tail], 3U);
    h_mouse_tail = (h_mouse_tail + 1U) % H_MOUSE_QUEUE_SIZE;
    return 0;
}

/* UART */
int openos_serial_read_byte(uint8_t *out)
{
    if (out == NULL || h_uart_rx_pos >= h_uart_rx_len) { return -1; }
    *out = h_uart_rx[h_uart_rx_pos++];
    return 0;
}

int openos_serial_write_byte(uint8_t byte)
{
    if (h_uart_tx_len >= H_UART_BUF_SIZE) { return -1; }
    h_uart_tx[h_uart_tx_len++] = byte;
    return 0;
}

/* ================================================================== */
/* Test-support helpers (inject / drain)                              */
/* ================================================================== */

int openos_harness_ata_write(uint32_t lba, const uint8_t *buf)
{
    return openos_ata_write(lba, buf);
}

int openos_harness_ata_read(uint32_t lba, uint8_t *buf)
{
    return openos_ata_read(lba, buf);
}

int openos_harness_ramdisk_write(uint32_t lba, const uint8_t *buf)
{
    return openos_ramdisk_write(lba, buf);
}

int openos_harness_ramdisk_read(uint32_t lba, uint8_t *buf)
{
    return openos_ramdisk_read(lba, buf);
}

int openos_harness_inject_rx_packet(const uint8_t *data, uint32_t len)
{
    return ring_push(h_rx_ring, &h_rx_head, &h_rx_tail, data, len);
}

int openos_harness_drain_tx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len)
{
    return ring_pop(h_tx_ring, &h_tx_head, &h_tx_tail, out, max_len, out_len);
}

int openos_harness_inject_mouse_packet(const uint8_t packet[3])
{
    unsigned next;
    if (packet == NULL) { return -1; }
    next = (h_mouse_head + 1U) % H_MOUSE_QUEUE_SIZE;
    if (next == h_mouse_tail) { return -1; }
    memcpy(h_mouse_queue[h_mouse_head], packet, 3U);
    h_mouse_head = next;
    return 0;
}

void openos_harness_feed_uart_rx(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0U || len > H_UART_BUF_SIZE) { return; }
    memcpy(h_uart_rx, data, len);
    h_uart_rx_len = len;
    h_uart_rx_pos = 0U;
}

size_t openos_harness_drain_uart_tx(uint8_t *out, size_t max_len)
{
    size_t n = h_uart_tx_len < max_len ? h_uart_tx_len : max_len;
    if (out != NULL && n > 0U) {
        memcpy(out, h_uart_tx, n);
    }
    h_uart_tx_len = 0U;
    return n;
}
