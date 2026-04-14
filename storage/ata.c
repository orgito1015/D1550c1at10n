#include "ata.h"
#include <stdint.h>
int ata_init(void){ return 0; }
int ata_read_sector(uint32_t lba, void* buffer){ return 0; }
int ata_write_sector(uint32_t lba, const void* buffer){ return 0; }
