#include "tests/test_common.h"

#include "mocks/mock_hw.h"
#include "storage/ata.h"
#include "storage/fat.h"
#include "storage/ramdisk.h"

#include <stdint.h>
#include <string.h>

#define FAT16_MAX_FILES 8

typedef struct fat16_mock_entry {
    char path[32];
    uint32_t start_lba;
    uint32_t size;
} fat16_mock_entry_t;

typedef struct fat16_mock_header {
    char magic[4];
    uint16_t file_count;
    fat16_mock_entry_t entries[FAT16_MAX_FILES];
} fat16_mock_header_t;

int test_ata_sector_rw(void) {
    uint8_t in[MOCK_SECTOR_SIZE];
    uint8_t out[MOCK_SECTOR_SIZE];
    uint32_t i;

    mock_hw_reset();
    ASSERT_TRUE(ata_init() == 0);

    for (i = 0; i < MOCK_SECTOR_SIZE; ++i) {
        in[i] = (uint8_t)(i & 0xFFU);
    }

    ASSERT_TRUE(ata_write_sector(7, in) == 0);
    ASSERT_TRUE(ata_read_sector(7, out) == 0);
    ASSERT_TRUE(memcmp(in, out, sizeof(in)) == 0);
    return 0;
}

int test_ramdisk_persistence(void) {
    uint8_t in[MOCK_SECTOR_SIZE];
    uint8_t out[MOCK_SECTOR_SIZE];
    memset(in, 0xAB, sizeof(in));

    mock_hw_reset();
    ASSERT_TRUE(ramdisk_init() == 0);
    ASSERT_TRUE(ramdisk_write_sector(3, in) == 0);
    ASSERT_TRUE(ramdisk_read_sector(3, out) == 0);
    ASSERT_TRUE(memcmp(in, out, sizeof(in)) == 0);
    return 0;
}

int test_fat16_read_file(void) {
    fat16_mock_header_t header;
    uint8_t sector[MOCK_SECTOR_SIZE];
    char output[32];
    size_t out_len = 0;
    const char *payload = "hello-fat16";

    mock_hw_reset();
    memset(&header, 0, sizeof(header));
    memcpy(header.magic, "MF16", 4);
    header.file_count = 1;
    memcpy(header.entries[0].path, "/hello.txt", 11);
    header.entries[0].start_lba = 1;
    header.entries[0].size = 11;

    memset(sector, 0, sizeof(sector));
    memcpy(sector, &header, sizeof(header));
    ASSERT_TRUE(ramdisk_write_sector(0, sector) == 0);

    memset(sector, 0, sizeof(sector));
    memcpy(sector, payload, 11);
    ASSERT_TRUE(ramdisk_write_sector(1, sector) == 0);

    ASSERT_TRUE(fat16_mount() == 0);
    ASSERT_TRUE(fat16_read("/hello.txt", output, sizeof(output), &out_len) == 0);
    ASSERT_TRUE(out_len == 11);
    ASSERT_TRUE(memcmp(output, payload, 11) == 0);
    return 0;
}
