#include <stdio.h>

int test_ata_sector_rw(void);
int test_ramdisk_persistence(void);
int test_fat16_read_file(void);
int test_nic_send_receive(void);
int test_arp_resolution(void);
int test_dhcp_mock_flow(void);
int test_framebuffer_pixel_draw(void);
int test_mouse_packet_movement(void);

typedef struct test_case {
    const char *name;
    int (*run)(void);
} test_case_t;

int main(void) {
    test_case_t tests[] = {
        {"ata_sector_rw", test_ata_sector_rw},
        {"ramdisk_persistence", test_ramdisk_persistence},
        {"fat16_read_file", test_fat16_read_file},
        {"nic_send_receive", test_nic_send_receive},
        {"arp_resolution", test_arp_resolution},
        {"dhcp_mock_flow", test_dhcp_mock_flow},
        {"framebuffer_pixel_draw", test_framebuffer_pixel_draw},
        {"mouse_packet_movement", test_mouse_packet_movement},
    };
    unsigned i;

    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        if (tests[i].run() != 0) {
            fprintf(stderr, "FAIL: %s\n", tests[i].name);
            return 1;
        }
        printf("PASS: %s\n", tests[i].name);
    }

    printf("All tests passed.\n");
    return 0;
}
