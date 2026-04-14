#ifndef STORAGE_RAMDISK_H
#define STORAGE_RAMDISK_H

#include <stdint.h>

int ramdisk_init(void);
int ramdisk_read_sector(uint32_t lba, void *buffer);
int ramdisk_write_sector(uint32_t lba, const void *buffer);

#endif
