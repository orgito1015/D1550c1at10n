#include "framework/irq_helpers.h"
#include "mocks/mock_hw.h"

void irq_register(int irq, void (*handler)(void)) { mock_irq_register(irq, handler); }
int irq_trigger(int irq) { return mock_irq_trigger(irq); }
