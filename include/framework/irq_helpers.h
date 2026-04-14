#ifndef FRAMEWORK_IRQ_HELPERS_H
#define FRAMEWORK_IRQ_HELPERS_H

void irq_register(int irq, void (*handler)(void));
int irq_trigger(int irq);

#endif
