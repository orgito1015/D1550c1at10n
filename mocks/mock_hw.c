#include "mocks/mock_hw.h"

#include <string.h>

#define MAX_IRQS 16

typedef struct packet_slot {
    uint8_t data[MOCK_MAX_PACKET_SIZE];
    uint32_t len;
    int used;
} packet_slot_t;

static uint8_t g_ports8[65536];
static uint16_t g_ports16[65536];
static uint32_t g_pci_devices = 1;
static mock_irq_handler_t g_irq_handlers[MAX_IRQS];

static uint32_t g_fb_width;
static uint32_t g_fb_height;
static uint32_t g_framebuffer[1024 * 768];

static uint8_t g_ata[MOCK_ATA_SECTORS][MOCK_SECTOR_SIZE];
static uint8_t g_ramdisk[MOCK_RAMDISK_SECTORS][MOCK_SECTOR_SIZE];

static packet_slot_t g_rx_ring[MOCK_PACKET_RING_SIZE];
static packet_slot_t g_tx_ring[MOCK_PACKET_RING_SIZE];
static unsigned g_rx_head;
static unsigned g_rx_tail;
static unsigned g_tx_head;
static unsigned g_tx_tail;

static uint8_t g_mouse_queue[32][3];
static unsigned g_mouse_head;
static unsigned g_mouse_tail;

static uint8_t g_uart_rx[256];
static size_t g_uart_rx_len;
static size_t g_uart_rx_pos;
static uint8_t g_uart_tx[256];
static size_t g_uart_tx_len;

void mock_hw_reset(void) {
    memset(g_ports8, 0, sizeof(g_ports8));
    memset(g_ports16, 0, sizeof(g_ports16));
    memset(g_irq_handlers, 0, sizeof(g_irq_handlers));
    memset(g_framebuffer, 0, sizeof(g_framebuffer));
    memset(g_ata, 0, sizeof(g_ata));
    memset(g_ramdisk, 0, sizeof(g_ramdisk));
    memset(g_rx_ring, 0, sizeof(g_rx_ring));
    memset(g_tx_ring, 0, sizeof(g_tx_ring));
    memset(g_mouse_queue, 0, sizeof(g_mouse_queue));
    memset(g_uart_rx, 0, sizeof(g_uart_rx));
    memset(g_uart_tx, 0, sizeof(g_uart_tx));
    g_pci_devices = 1;
    g_fb_width = 640;
    g_fb_height = 480;
    g_rx_head = g_rx_tail = 0;
    g_tx_head = g_tx_tail = 0;
    g_mouse_head = g_mouse_tail = 0;
    g_uart_rx_len = g_uart_rx_pos = 0;
    g_uart_tx_len = 0;
}

uint8_t mock_inb(uint16_t port) { return g_ports8[port]; }
void mock_outb(uint16_t port, uint8_t value) { g_ports8[port] = value; }
uint16_t mock_inw(uint16_t port) { return g_ports16[port]; }
void mock_outw(uint16_t port, uint16_t value) { g_ports16[port] = value; }

void mock_pci_set_device_count(uint32_t device_count) { g_pci_devices = device_count; }
uint32_t mock_pci_scan(void) { return g_pci_devices; }

void mock_irq_register(int irq, mock_irq_handler_t handler) {
    if (irq >= 0 && irq < MAX_IRQS) {
        g_irq_handlers[irq] = handler;
    }
}

int mock_irq_trigger(int irq) {
    if (irq < 0 || irq >= MAX_IRQS || g_irq_handlers[irq] == 0) {
        return -1;
    }
    g_irq_handlers[irq]();
    return 0;
}

int mock_framebuffer_init(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0 || width > 1024 || height > 768) {
        return -1;
    }
    g_fb_width = width;
    g_fb_height = height;
    memset(g_framebuffer, 0, sizeof(g_framebuffer));
    return 0;
}

uint32_t *mock_framebuffer_data(void) { return g_framebuffer; }
uint32_t mock_framebuffer_width(void) { return g_fb_width; }
uint32_t mock_framebuffer_height(void) { return g_fb_height; }

int mock_ata_read_sector(uint32_t lba, uint8_t *buffer) {
    if (lba >= MOCK_ATA_SECTORS || buffer == 0) {
        return -1;
    }
    memcpy(buffer, g_ata[lba], MOCK_SECTOR_SIZE);
    return 0;
}

int mock_ata_write_sector(uint32_t lba, const uint8_t *buffer) {
    if (lba >= MOCK_ATA_SECTORS || buffer == 0) {
        return -1;
    }
    memcpy(g_ata[lba], buffer, MOCK_SECTOR_SIZE);
    return 0;
}

int mock_ramdisk_read_sector(uint32_t lba, uint8_t *buffer) {
    if (lba >= MOCK_RAMDISK_SECTORS || buffer == 0) {
        return -1;
    }
    memcpy(buffer, g_ramdisk[lba], MOCK_SECTOR_SIZE);
    return 0;
}

int mock_ramdisk_write_sector(uint32_t lba, const uint8_t *buffer) {
    if (lba >= MOCK_RAMDISK_SECTORS || buffer == 0) {
        return -1;
    }
    memcpy(g_ramdisk[lba], buffer, MOCK_SECTOR_SIZE);
    return 0;
}

static int ring_push(packet_slot_t *ring, unsigned *head, unsigned *tail, const uint8_t *data, uint32_t len) {
    unsigned next = (*head + 1U) % MOCK_PACKET_RING_SIZE;
    if (next == *tail || data == 0 || len == 0 || len > MOCK_MAX_PACKET_SIZE) {
        return -1;
    }
    memcpy(ring[*head].data, data, len);
    ring[*head].len = len;
    ring[*head].used = 1;
    *head = next;
    return 0;
}

static int ring_pop(packet_slot_t *ring, unsigned *head, unsigned *tail, uint8_t *out, uint32_t max_len, uint32_t *out_len) {
    if (*tail == *head || out == 0 || out_len == 0) {
        return -1;
    }
    if (!ring[*tail].used || ring[*tail].len > max_len) {
        return -1;
    }
    memcpy(out, ring[*tail].data, ring[*tail].len);
    *out_len = ring[*tail].len;
    ring[*tail].used = 0;
    *tail = (*tail + 1U) % MOCK_PACKET_RING_SIZE;
    return 0;
}

int mock_nic_push_rx_packet(const uint8_t *data, uint32_t len) { return ring_push(g_rx_ring, &g_rx_head, &g_rx_tail, data, len); }
int mock_nic_pop_rx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len) { return ring_pop(g_rx_ring, &g_rx_head, &g_rx_tail, out, max_len, out_len); }
int mock_nic_push_tx_packet(const uint8_t *data, uint32_t len) { return ring_push(g_tx_ring, &g_tx_head, &g_tx_tail, data, len); }
int mock_nic_pop_tx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len) { return ring_pop(g_tx_ring, &g_tx_head, &g_tx_tail, out, max_len, out_len); }

int mock_mouse_push_packet(const uint8_t packet[3]) {
    unsigned next = (g_mouse_head + 1U) % 32U;
    if (packet == 0 || next == g_mouse_tail) {
        return -1;
    }
    memcpy(g_mouse_queue[g_mouse_head], packet, 3);
    g_mouse_head = next;
    return 0;
}

int mock_mouse_pop_packet(uint8_t packet[3]) {
    if (packet == 0 || g_mouse_head == g_mouse_tail) {
        return -1;
    }
    memcpy(packet, g_mouse_queue[g_mouse_tail], 3);
    g_mouse_tail = (g_mouse_tail + 1U) % 32U;
    return 0;
}

void mock_uart_feed_rx(const uint8_t *data, size_t len) {
    if (data == 0 || len == 0 || len > sizeof(g_uart_rx)) {
        return;
    }
    memcpy(g_uart_rx, data, len);
    g_uart_rx_len = len;
    g_uart_rx_pos = 0;
}

int mock_uart_read_byte(uint8_t *out_byte) {
    if (out_byte == 0 || g_uart_rx_pos >= g_uart_rx_len) {
        return -1;
    }
    *out_byte = g_uart_rx[g_uart_rx_pos++];
    return 0;
}

int mock_uart_write_byte(uint8_t byte) {
    if (g_uart_tx_len >= sizeof(g_uart_tx)) {
        return -1;
    }
    g_uart_tx[g_uart_tx_len++] = byte;
    return 0;
}

size_t mock_uart_take_tx(uint8_t *out, size_t max_len) {
    size_t to_copy = g_uart_tx_len < max_len ? g_uart_tx_len : max_len;
    if (out != 0 && to_copy > 0) {
        memcpy(out, g_uart_tx, to_copy);
    }
    g_uart_tx_len = 0;
    return to_copy;
}
