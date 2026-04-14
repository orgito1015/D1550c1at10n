#include "framework/io.h"
#include "mocks/mock_hw.h"

uint8_t inb(uint16_t port) { return mock_inb(port); }
void outb(uint16_t port, uint8_t value) { mock_outb(port, value); }
uint16_t inw(uint16_t port) { return mock_inw(port); }
void outw(uint16_t port, uint16_t value) { mock_outw(port, value); }
