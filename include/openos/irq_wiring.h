#ifndef OPENOS_IRQ_WIRING_H
#define OPENOS_IRQ_WIRING_H

/*
 * openos_irq_wire_all()
 *
 * Registers the NIC (IRQ 11), PS/2 mouse (IRQ 12), and UART (IRQ 4)
 * interrupt handlers.  Call once after openos_drivers_init() succeeds.
 *
 * All three subsystems remain functional through their polling APIs
 * (rtl8139_receive_packet, mouse_poll, uart_receive_byte) if the
 * hardware IRQ lines are not yet active.
 */
void openos_irq_wire_all(void);

#endif /* OPENOS_IRQ_WIRING_H */
