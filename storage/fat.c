#include "storage/fat.h"
#include "storage/ramdisk.h"
#include "mocks/mock_hw.h"

#include <stdint.h>
#include <string.h>

#define FAT16_MOCK_MAGIC "MF16"
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

static fat16_mock_header_t g_header;
static int g_mounted;

int fat16_mount(void) {
    uint8_t sector[MOCK_SECTOR_SIZE];
    if (ramdisk_read_sector(0, sector) != 0) {
        return -1;
    }
    memcpy(&g_header, sector, sizeof(g_header));
    if (memcmp(g_header.magic, FAT16_MOCK_MAGIC, 4) != 0 || g_header.file_count > FAT16_MAX_FILES) {
        g_mounted = 0;
        return -1;
    }
    g_mounted = 1;
    return 0;
}

int fat16_read(const char *path, void *out, size_t out_size, size_t *out_len) {
    uint8_t sector[MOCK_SECTOR_SIZE];
    uint32_t idx;
    uint32_t remaining;
    uint8_t *cursor = (uint8_t *)out;

    if (!g_mounted || path == 0 || out == 0 || out_len == 0) {
        return -1;
    }

    for (idx = 0; idx < g_header.file_count; ++idx) {
        if (strncmp(path, g_header.entries[idx].path, sizeof(g_header.entries[idx].path)) == 0) {
            uint32_t lba = g_header.entries[idx].start_lba;
            remaining = g_header.entries[idx].size;
            if (remaining > out_size) {
                return -1;
            }
            while (remaining > 0) {
                uint32_t chunk = remaining > MOCK_SECTOR_SIZE ? MOCK_SECTOR_SIZE : remaining;
                if (ramdisk_read_sector(lba++, sector) != 0) {
                    return -1;
                }
                memcpy(cursor, sector, chunk);
                cursor += chunk;
                remaining -= chunk;
            }
            *out_len = g_header.entries[idx].size;
            return 0;
        }
    }
    return -1;
}
