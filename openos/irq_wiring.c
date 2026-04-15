/*
 * openos/irq_wiring.c
 *
 * Connects hardware interrupt lines to driver dispatch functions.
 *
 * Call openos_irq_wire_all() once, after openos_drivers_init() has
 * successfully completed, to enable interrupt-driven operation for the
 * NIC, PS/2 mouse, and UART subsystems.
 *
 * Polling fallbacks
 * -----------------
 * Where IRQ registration is unavailable (openos_irq_register returns
 * without effect because the kernel has not yet enabled the PIC/APIC),
 * the drivers remain fully functional via their respective poll
 * functions:
 *   - Network  : rtl8139_receive_packet()
 *   - Mouse    : mouse_poll()
 *   - UART     : uart_receive_byte()
 *
 * IRQ assignments (PC/AT defaults)
 * ---------------------------------
 *   IRQ  4 – COM1 / UART
 *   IRQ 11 – RTL8139 NIC (PCI shared)
 *   IRQ 12 – PS/2 auxiliary (mouse)
 */

#include "openos/irq_wiring.h"
#include "framework/irq_helpers.h"
#include "network/rtl8139.h"
#include "input/mouse.h"
#include "input/uart.h"

#define IRQ_UART   4
#define IRQ_NIC   11
#define IRQ_MOUSE 12

/* ------------------------------------------------------------------ */
/* IRQ handler stubs                                                   */
/* ------------------------------------------------------------------ */

/*
 * NIC receive IRQ handler.
 *
 * In a real driver this would read the RTL8139 ISR register to confirm
 * a RX-OK event, then drain packets from the receive ring into the
 * kernel's network stack.  Here we call the driver's polling receive
 * path so the handler is self-contained and testable.
 */
static void nic_irq_handler(void)
{
    uint8_t  buf[1536];
    int      len = rtl8139_receive_packet(buf, sizeof(buf));
    /* A production kernel would forward `buf`/`len` to the IP stack. */
    (void)len;
}

/*
 * PS/2 mouse IRQ handler (IRQ 12).
 *
 * Reads the next decoded mouse packet and forwards position/button
 * deltas to the kernel's input event queue.
 */
static void mouse_irq_handler(void)
{
    mouse_state_t state;
    int ret = mouse_poll(&state);
    /* A production kernel would enqueue the event for userspace. */
    (void)ret;
}

/*
 * UART receive IRQ handler (IRQ 4 / COM1).
 *
 * Drains one byte from the UART FIFO and forwards it to the kernel's
 * serial line discipline.
 */
static void uart_irq_handler(void)
{
    uint8_t byte;
    int ret = uart_receive_byte(&byte);
    /* A production kernel would push the byte into the tty buffer. */
    (void)ret;
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void openos_irq_wire_all(void)
{
    irq_register(IRQ_UART,  uart_irq_handler);
    irq_register(IRQ_NIC,   nic_irq_handler);
    irq_register(IRQ_MOUSE, mouse_irq_handler);
}
