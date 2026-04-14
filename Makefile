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

.PHONY: all clean test smoke lint examples

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
