#ifndef STORAGE_FAT_H
#define STORAGE_FAT_H

#include <stddef.h>
#include <stdint.h>

int fat16_mount(void);
int fat16_read(const char *path, void *out, size_t out_size, size_t *out_len);

#endif
