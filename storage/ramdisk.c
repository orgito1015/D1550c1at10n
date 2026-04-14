#include "storage/ramdisk.h"
#include "mocks/mock_hw.h"

int ramdisk_init(void) { return 0; }

int ramdisk_read_sector(uint32_t lba, void *buffer) {
    return mock_ramdisk_read_sector(lba, (uint8_t *)buffer);
}

int ramdisk_write_sector(uint32_t lba, const void *buffer) {
    return mock_ramdisk_write_sector(lba, (const uint8_t *)buffer);
}
