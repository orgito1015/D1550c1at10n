#include "storage/ata.h"
#include "mocks/mock_hw.h"

int ata_init(void) { return 0; }

int ata_read_sector(uint32_t lba, void *buffer) {
    return mock_ata_read_sector(lba, (uint8_t *)buffer);
}

int ata_write_sector(uint32_t lba, const void *buffer) {
    return mock_ata_write_sector(lba, (const uint8_t *)buffer);
}
