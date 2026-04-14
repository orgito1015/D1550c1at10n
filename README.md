# D1550c1at10n

D1550c1at10n is now a standalone driver-development framework extracted from OpenOS-oriented code and rebuilt for independent compilation, simulation, and validation.

## Architecture

```text
D1550c1at10n/
├── include/     # Public module + framework interfaces
├── framework/   # Core abstractions (I/O, IRQ, PCI)
├── storage/     # ATA, RAM disk, FAT16 mock filesystem reader
├── network/     # RTL8139 simulation, ARP, DHCP mock flow
├── display/     # VBE framebuffer, font stub, cursor state
├── input/       # PS/2 mouse decode, UART abstraction
├── mocks/       # Hardware emulation backing all modules
├── tests/       # Unit tests + smoke test entrypoint
├── examples/    # Runnable demos independent of OpenOS
├── docs/        # Additional architecture notes
└── Makefile
```

## Build

```bash
make all
```

## Test

```bash
make test
```

## Smoke examples

```bash
make smoke
```

## Lint/check build flags

```bash
make lint
```

## Module summary

- **Storage:** mock-backed ATA sector IO, RAM disk sector IO, FAT16-style mock mount/read API.
- **Network:** RTL8139 TX/RX ring simulation using mock queues, ARP request/reply and cache, DHCP discover/request flow.
- **Display:** framebuffer init + pixel writes, simple font rendering stub, cursor tracking.
- **Input:** PS/2 packet decoding into mouse state, UART send/receive backed by mock buffers.

## OpenOS integration notes

The interfaces are intentionally clean and boundary-based. To integrate with OpenOS later:
1. Replace `mocks/mock_hw.c` bindings with real kernel HAL calls.
2. Keep driver APIs in `include/` stable so tests remain reusable.
3. Reuse unit tests by swapping mock hooks for OpenOS test harness adapters.

See `/docs/ARCHITECTURE.md` for dependency boundaries.
