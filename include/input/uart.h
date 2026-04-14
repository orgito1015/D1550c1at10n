#ifndef INPUT_UART_H
#define INPUT_UART_H

#include <stdint.h>

int uart_init(void);
int uart_send_byte(uint8_t byte);
int uart_receive_byte(uint8_t *byte);

#endif
