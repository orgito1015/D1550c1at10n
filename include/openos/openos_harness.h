#ifndef OPENOS_OPENOS_HARNESS_H
#define OPENOS_OPENOS_HARNESS_H

/*
 * OpenOS test harness
 *
 * Provides an in-process implementation of every openos_* function
 * declared in openos/hal.h.  The harness backs each subsystem with the
 * same deterministic arrays used by mocks/mock_hw.c, making it possible
 * to compile and run the OpenOS integration tests without a real kernel.
 *
 * Linking model for integration tests
 * ------------------------------------
 *   driver sources  (ata.c, rtl8139.c, vbe.c, …)
 *     → openos/hal.c          (mock_* → openos_*)
 *     → openos/openos_harness.c  (openos_* → in-process arrays)
 *
 * mocks/mock_hw.c is NOT linked; the harness provides all openos_*
 * symbols directly.
 *
 * Test-support helpers
 * --------------------
 * The harness exposes additional helpers so test code can inject state
 * (e.g. pre-load a sector, push an RX packet, feed UART bytes) without
 * touching private arrays.
 */

#include <stddef.h>
#include <stdint.h>

/* Reset all harness state (call between test cases). */
void openos_harness_reset(void);

/* ------------------------------------------------------------------ */
/* Storage helpers                                                     */
/* ------------------------------------------------------------------ */
int openos_harness_ata_write(uint32_t lba, const uint8_t *buf);
int openos_harness_ata_read(uint32_t lba, uint8_t *buf);
int openos_harness_ramdisk_write(uint32_t lba, const uint8_t *buf);
int openos_harness_ramdisk_read(uint32_t lba, uint8_t *buf);

/* ------------------------------------------------------------------ */
/* Network helpers                                                     */
/* ------------------------------------------------------------------ */
/* Inject a packet into the NIC receive ring (simulates hardware RX). */
int openos_harness_inject_rx_packet(const uint8_t *data, uint32_t len);
/* Drain one packet from the NIC transmit ring (inspect what was sent). */
int openos_harness_drain_tx_packet(uint8_t *out, uint32_t max_len, uint32_t *out_len);

/* ------------------------------------------------------------------ */
/* Input helpers                                                       */
/* ------------------------------------------------------------------ */
/* Push a 3-byte PS/2 packet into the mouse queue. */
int openos_harness_inject_mouse_packet(const uint8_t packet[3]);
/* Pre-load bytes into the UART RX buffer. */
void openos_harness_feed_uart_rx(const uint8_t *data, size_t len);
/* Drain all bytes written to the UART TX buffer. */
size_t openos_harness_drain_uart_tx(uint8_t *out, size_t max_len);

#endif /* OPENOS_OPENOS_HARNESS_H */
