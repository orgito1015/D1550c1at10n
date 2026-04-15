CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -Iinclude -I.

SRC := \
framework/io.c \
framework/pci.c \
framework/irq_helpers.c \
storage/ata.c \
storage/ramdisk.c \
storage/fat.c \
network/rtl8139.c \
network/arp.c \
network/dhcp.c \
display/vbe.c \
display/font.c \
display/cursor.c \
input/mouse.c \
input/uart.c \
mocks/mock_hw.c

TEST_SRC := tests/test_main.c tests/test_storage.c tests/test_network.c tests/test_display_input.c
EXAMPLE_SRC := \
examples/disk_read_test.c \
examples/network_packet_simulation.c \
examples/framebuffer_draw_demo.c \
examples/mouse_movement_simulation.c

BUILD_DIR := build
EXAMPLES_DIR := $(BUILD_DIR)/examples
UNIT_TEST := $(BUILD_DIR)/unit_tests
LIB := $(BUILD_DIR)/libd1550c1at10n.a

OBJ := $(SRC:%.c=$(BUILD_DIR)/%.o)
TEST_OBJ := $(TEST_SRC:%.c=$(BUILD_DIR)/%.o)

EXAMPLE_BINS := $(patsubst examples/%.c,$(EXAMPLES_DIR)/%,$(EXAMPLE_SRC))

# ------------------------------------------------------------------
# OpenOS integration build
#
# OPENOS_SRC replaces mocks/mock_hw.c with openos/hal.c and adds the
# OpenOS-specific modules (driver manager, drivers init, IRQ wiring,
# and the test harness).  The driver and framework sources are shared
# with the standard build.
# ------------------------------------------------------------------
OPENOS_DRIVER_SRC := \
framework/io.c \
framework/pci.c \
framework/irq_helpers.c \
storage/ata.c \
storage/ramdisk.c \
storage/fat.c \
network/rtl8139.c \
network/arp.c \
network/dhcp.c \
display/vbe.c \
display/font.c \
display/cursor.c \
input/mouse.c \
input/uart.c \
openos/hal.c \
openos/driver_manager.c \
openos/drivers_init.c \
openos/irq_wiring.c \
openos/openos_harness.c

OPENOS_TEST_SRC := tests/test_openos_integration.c

OPENOS_BUILD_DIR  := $(BUILD_DIR)/openos
OPENOS_LIB        := $(OPENOS_BUILD_DIR)/libopenos_drivers.a
OPENOS_TEST_BIN   := $(OPENOS_BUILD_DIR)/openos_integration_tests

OPENOS_OBJ      := $(OPENOS_DRIVER_SRC:%.c=$(OPENOS_BUILD_DIR)/%.o)
OPENOS_TEST_OBJ := $(OPENOS_TEST_SRC:%.c=$(OPENOS_BUILD_DIR)/%.o)

.PHONY: all clean test smoke lint examples openos-lib openos-test openos-smoke

all: $(LIB) $(UNIT_TEST) examples

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	ar rcs $@ $^

$(UNIT_TEST): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

test: $(UNIT_TEST)
	./$(UNIT_TEST)

examples: $(EXAMPLE_BINS)

$(EXAMPLES_DIR)/%: examples/%.c $(OBJ)
	@mkdir -p $(EXAMPLES_DIR)
	$(CC) $(CFLAGS) $< $(OBJ) -o $@

smoke: examples
	@for example in $(EXAMPLE_BINS); do \
		echo "Running $$example"; \
		./$$example; \
	done

lint:
	$(MAKE) clean
	$(MAKE) all CFLAGS="-std=c11 -Wall -Wextra -Werror -pedantic -Iinclude -I."

clean:
	rm -rf $(BUILD_DIR)

# ------------------------------------------------------------------
# OpenOS targets
# ------------------------------------------------------------------

$(OPENOS_BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Static library containing all OpenOS-backed driver objects.
openos-lib: $(OPENOS_LIB)

$(OPENOS_LIB): $(OPENOS_OBJ)
	ar rcs $@ $^

# Build and run the OpenOS integration test suite.
openos-test: $(OPENOS_TEST_BIN)
	./$(OPENOS_TEST_BIN)

$(OPENOS_TEST_BIN): $(OPENOS_OBJ) $(OPENOS_TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Smoke-run every example linked against the OpenOS HAL.
openos-smoke: openos-lib
	@for example in $(EXAMPLE_BINS); do \
		src=examples/$$(basename $$example).c; \
		out=$(OPENOS_BUILD_DIR)/smoke_$$(basename $$example); \
		$(CC) $(CFLAGS) $$src $(OPENOS_OBJ) -o $$out; \
		echo "Running $$out"; \
		./$$out; \
	done
