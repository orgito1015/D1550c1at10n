#include "input/uart.h"
#include "mocks/mock_hw.h"

int uart_init(void) { return 0; }
int uart_send_byte(uint8_t byte) { return mock_uart_write_byte(byte); }
int uart_receive_byte(uint8_t *byte) { return mock_uart_read_byte(byte); }
