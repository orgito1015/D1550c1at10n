# Architecture Notes

## Layering

1. **Drivers/Protocols** (`storage`, `network`, `display`, `input`) depend only on public headers.
2. **Framework adapters** (`framework`) abstract low-level system hooks.
3. **Mock hardware layer** (`mocks/mock_hw.c`) provides deterministic behavior for standalone validation.

## Data flow examples

- ATA and RAM disk calls route to sector arrays in mock hardware.
- RTL8139 uses TX/RX ring queues implemented in the mock layer.
- VBE APIs draw into mock framebuffer memory.
- Mouse/UART consume and emit bytes from mock input/output queues.

## Testing strategy

Unit tests validate each subsystem in isolation using deterministic mock state reset (`mock_hw_reset`).
