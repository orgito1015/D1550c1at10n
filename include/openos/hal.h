#ifndef OPENOS_HAL_H
#define OPENOS_HAL_H

/*
 * OpenOS HAL service contracts
 *
 * These functions must be provided by the OpenOS kernel.  The OpenOS HAL
 * implementation (openos/hal.c) calls them to satisfy the driver-facing
 * mock_hw.h interface, replacing mocks/mock_hw.c at link time.
 *
 * Port I/O and IRQ functions map directly to x86 in/out instructions and
 * the kernel's IDT registration helpers.  Storage, NIC, framebuffer, and
 * input entries delegate to the corresponding kernel subsystem APIs.
 */

#include <stddef.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Port I/O                                                            */
/* ------------------------------------------------------------------ */
uint8_t  openos_inb(uint16_t port);
void     openos_outb(uint16_t port, uint8_t value);
uint16_t openos_inw(uint16_t port);
void     openos_outw(uint16_t port, uint16_t value);

/* ------------------------------------------------------------------ */
/* PCI enumeration                                                     */
/* ------------------------------------------------------------------ */
/* Returns the number of PCI devices detected during bus scan. */
uint32_t openos_pci_device_count(void);

/* ------------------------------------------------------------------ */
/* IRQ registration                                                    */
/* ------------------------------------------------------------------ */
/*
 * Register a handler for the given IRQ line.  The kernel installs the
 * handler into the IDT and enables the line on the PIC/APIC.
 */
void openos_irq_register(int irq, void (*handler)(void));

/* ------------------------------------------------------------------ */
/* Framebuffer / display                                               */
/* ------------------------------------------------------------------ */
/*
 * Request a linear framebuffer with the given pixel dimensions.
 * Returns 0 on success, -1 if the mode is unavailable.
 */
int       openos_framebuffer_request(uint32_t width, uint32_t height);
/* Returns the kernel-mapped base address of the framebuffer. */
uint32_t *openos_framebuffer_addr(void);
/* Returns the actual width and height granted by the kernel. */
uint32_t  openos_framebuffer_width(void);
uint32_t  openos_framebuffer_height(void);

/* ------------------------------------------------------------------ */
/* Storage: ATA block device                                           */
/* ------------------------------------------------------------------ */
int openos_ata_read(uint32_t lba, uint8_t *buf);
int openos_ata_write(uint32_t lba, const uint8_t *buf);

/* ------------------------------------------------------------------ */
/* Storage: RAM disk                                                   */
/* ------------------------------------------------------------------ */
int openos_ramdisk_read(uint32_t lba, uint8_t *buf);
int openos_ramdisk_write(uint32_t lba, const uint8_t *buf);

/* ------------------------------------------------------------------ */
/* Network: NIC (RTL8139)                                              */
/* ------------------------------------------------------------------ */
/* Enqueue a packet for transmission; returns 0 on success. */
int openos_nic_tx(const uint8_t *data, uint32_t len);
/*
 * Dequeue the next received packet into *out (at most max_len bytes).
 * Writes the actual length to *out_len.  Returns 0 if a packet was
 * available, -1 if the RX queue is empty.
 */
int openos_nic_rx(uint8_t *out, uint32_t max_len, uint32_t *out_len);

/* ------------------------------------------------------------------ */
/* Input: PS/2 mouse                                                   */
/* ------------------------------------------------------------------ */
/*
 * Read the next 3-byte PS/2 packet from the kernel's mouse ring buffer.
 * Returns 0 if a packet was available, -1 if the queue is empty.
 */
int openos_mouse_read_packet(uint8_t packet[3]);

/* ------------------------------------------------------------------ */
/* Input: UART / serial (COM1)                                         */
/* ------------------------------------------------------------------ */
int openos_serial_read_byte(uint8_t *out);
int openos_serial_write_byte(uint8_t byte);

#endif /* OPENOS_HAL_H */
