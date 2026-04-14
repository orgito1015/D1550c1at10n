#include "network/arp.h"

#include <string.h>

#define ARP_ETHERTYPE 0x0806
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_TABLE_SIZE 8

typedef struct arp_entry {
    uint32_t ip;
    uint8_t mac[6];
    int valid;
} arp_entry_t;

static uint32_t g_local_ip = 0xC0A8010AU;
static uint8_t g_local_mac[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
static arp_entry_t g_table[ARP_TABLE_SIZE];

static uint16_t bswap16(uint16_t v) { return (uint16_t)((v >> 8) | (v << 8)); }

void arp_set_local_identity(uint32_t local_ip, const uint8_t local_mac[6]) {
    g_local_ip = local_ip;
    memcpy(g_local_mac, local_mac, 6);
    memset(g_table, 0, sizeof(g_table));
}

int arp_send_request(uint32_t ip, uint8_t *out_frame, size_t *out_len) {
    size_t i;
    uint16_t *u16;
    if (out_frame == 0 || out_len == 0) {
        return -1;
    }

    memset(out_frame, 0, 42);
    for (i = 0; i < 6; ++i) {
        out_frame[i] = 0xFF;
    }
    memcpy(out_frame + 6, g_local_mac, 6);
    u16 = (uint16_t *)(out_frame + 12);
    u16[0] = bswap16(ARP_ETHERTYPE);
    u16 = (uint16_t *)(out_frame + 14);
    u16[0] = bswap16(1);
    u16[1] = bswap16(0x0800);
    out_frame[18] = 6;
    out_frame[19] = 4;
    u16[3] = bswap16(ARP_REQUEST);
    memcpy(out_frame + 22, g_local_mac, 6);
    memcpy(out_frame + 28, &g_local_ip, 4);
    memcpy(out_frame + 38, &ip, 4);
    *out_len = 42;
    return 0;
}

static void arp_table_store(uint32_t ip, const uint8_t mac[6]) {
    unsigned i;
    for (i = 0; i < ARP_TABLE_SIZE; ++i) {
        if (!g_table[i].valid || g_table[i].ip == ip) {
            g_table[i].valid = 1;
            g_table[i].ip = ip;
            memcpy(g_table[i].mac, mac, 6);
            return;
        }
    }
}

int arp_handle_packet(const uint8_t *frame, size_t len, uint8_t *out_reply, size_t *out_reply_len) {
    uint16_t opcode;
    uint32_t sender_ip;
    uint32_t target_ip;

    if (frame == 0 || len < 42) {
        return -1;
    }

    opcode = (uint16_t)((frame[20] << 8) | frame[21]);
    memcpy(&sender_ip, frame + 28, 4);
    memcpy(&target_ip, frame + 38, 4);
    arp_table_store(sender_ip, frame + 22);

    if (opcode == ARP_REQUEST && target_ip == g_local_ip && out_reply != 0 && out_reply_len != 0) {
        memcpy(out_reply, frame + 22, 6);
        memcpy(out_reply + 6, g_local_mac, 6);
        out_reply[12] = 0x08;
        out_reply[13] = 0x06;
        out_reply[14] = 0x00;
        out_reply[15] = 0x01;
        out_reply[16] = 0x08;
        out_reply[17] = 0x00;
        out_reply[18] = 0x06;
        out_reply[19] = 0x04;
        out_reply[20] = 0x00;
        out_reply[21] = ARP_REPLY;
        memcpy(out_reply + 22, g_local_mac, 6);
        memcpy(out_reply + 28, &g_local_ip, 4);
        memcpy(out_reply + 32, frame + 22, 6);
        memcpy(out_reply + 38, &sender_ip, 4);
        *out_reply_len = 42;
        return 1;
    }

    return 0;
}

int arp_resolve(uint32_t ip, uint8_t out_mac[6]) {
    unsigned i;
    if (out_mac == 0) {
        return -1;
    }
    for (i = 0; i < ARP_TABLE_SIZE; ++i) {
        if (g_table[i].valid && g_table[i].ip == ip) {
            memcpy(out_mac, g_table[i].mac, 6);
            return 0;
        }
    }
    return -1;
}
