#include "storage/ata.h"
#include "mocks/mock_hw.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    unsigned char buffer[MOCK_SECTOR_SIZE];
    memset(buffer, 0, sizeof(buffer));

    mock_hw_reset();
    strcpy((char *)buffer, "disk example");
    (void)ata_write_sector(1, buffer);
    memset(buffer, 0, sizeof(buffer));
    (void)ata_read_sector(1, buffer);
    printf("ATA sector[1]: %s\n", buffer);
    return 0;
}
